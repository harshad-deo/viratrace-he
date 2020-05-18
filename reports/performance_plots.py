import itertools
import json
import logging
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import pymc3 as pm
import re
import statsmodels.api as sm

logging.basicConfig(format='%(asctime)s - [%(lineno)4d]:[%(levelname)5s] - %(message)s', level=logging.INFO)
log = logging.getLogger('performance-plot')
BATCH_SIZE = 4096


def make_qqplot(title, data_grouped):
    n_rows = (len(data_grouped) + 1) // 2
    fig, axs = plt.subplots(n_rows, 2, figsize=(16, 8 * n_rows))

    for (idx, group) in enumerate(data_grouped):
        row = idx // 2
        col = idx % 2
        ax = axs[row, col]
        scale, series = group
        ax.set_title(f'scale: {scale * BATCH_SIZE}', fontsize=18)
        sm.qqplot(data=series, ax=ax, line='s')

    if len(data_grouped) % 2 == 1:
        axs[-1, -1].axis('off')

    fig.suptitle(f'QQPlot: {title}', fontsize=20)
    fig.savefig(f'qqplot_{title}.svg')


def calibrate_model(data_grouped, data):
    y_idx = np.array([x['idx'] for x in data])
    mean_scale = np.array([x['size'] for x in data]) * BATCH_SIZE
    sd_scale = np.sqrt(mean_scale)
    y_obs = np.array([x['real_time'] for x in data])
    with pm.Model() as model:
        num_groups = len(data_grouped)
        # hierarchical parameters
        mu_0 = pm.Normal('mu_0', mu=1.3, sd=0.1)
        sd_0 = pm.HalfNormal('sd_0', sd=0.2)
        # population parameters
        mu = pm.Normal('mu', mu=mu_0, sd=sd_0, shape=num_groups)
        sd = pm.HalfNormal('sd', sd=0.1)
        mu_scaled = pm.Deterministic('mu_scaled', mu[y_idx] * mean_scale)
        sd_scaled = pm.Deterministic('sd_scaled', sd * sd_scale)

        nu_minus_1 = pm.Exponential('nu_minus_1', lam=0.1)
        nu = pm.Deterministic('nu', 1 + nu_minus_1)  # ensures dof >= 1

        y_pred = pm.StudentT('y_pred', mu=mu_scaled, sd=sd_scaled, nu=nu, observed=y_obs)

        trace = pm.sample(7000, chains=6, cores=6)

    chain = trace[2000:]
    return chain


def make_posterior_plots(data_grouped, chain, title):
    fig, axs = plt.subplots(3, 2, figsize=(16, 18))
    titles = ['mu_0'] + [f'mu_{x[0] * BATCH_SIZE}' for x in data_grouped]

    axs = pm.plot_posterior(chain, var_names=['mu_0', 'mu'], credible_interval=0.95, ax=axs, round_to=4, kind='hist')

    ax_idx = 0
    for row in axs:
        for ax in row:
            ax.set_title(titles[ax_idx], fontsize=18)
            ax_idx += 1

    _ = fig.suptitle(f'Amortised Operation Time (μs) - {title}', fontsize=20)
    fig.savefig(f'operation_time_{title}.svg')


def make_plots(file_name, title):
    log.info(f'initiating for {title}')
    with open(file_name, 'r') as f:
        data = json.load(f)
    data = data['benchmarks']
    data = filter(lambda x: x['run_type'] == 'iteration', data)
    tmp = []
    idxs = {}
    idx_ctr = 0
    for x in data:
        size = x['name']
        size = re.findall(r'8192/(\d{1,2})', size)
        size = int(size[0])
        real_time = x['real_time'] * 1000

        if size in idxs:
            idx = idxs[size]
        else:
            idx = idx_ctr
            idxs[size] = idx_ctr
            idx_ctr += 1

        res = {"size": size, "idx": idx, "real_time": real_time}
        tmp.append(res)

    data = tmp
    data_grouped = itertools.groupby(data, lambda x: x['size'])

    def map_seq(size, xs):
        xs = map(lambda x: x['real_time'], xs)
        return (size, np.array(list(xs)))

    data_grouped = list(itertools.starmap(map_seq, data_grouped))

    log.info(' > making qq plot')
    make_qqplot(title, data_grouped)
    log.info(' > calibrating model')
    chain = calibrate_model(data_grouped, data)
    log.info(' > making posterior plots')
    make_posterior_plots(data_grouped, chain, title)


if __name__ == '__main__':
    matplotlib.use('Agg')
    make_plots('data/encode.json', 'encode')
    make_plots('data/multiply.json', 'multiply')
    make_plots('data/decrypt.json', 'decrypt')
