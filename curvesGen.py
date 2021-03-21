#!/usr/bin/env python
# -*- coding: utf-8 -*-

import plotly.offline as py
import plotly.graph_objs as go
import pandas as pd
import matplotlib
import matplotlib.pyplot as plt
import numpy as np

data = pd.read_csv('results/gain.csv')
data.head()

#************************ PLOTLY ************************

# x = [1, 2, 3, 4, 5, 6]
# x_rev = x[::-1]

# # Ca+NCX
# y1 = data['Ca+NCX'].values
# y1_upper = []
# y1_lower = []
# for i in y1:
#     y1_upper.append(i+1)
#     y1_lower.append(i-1)
# y1_lower = y1_lower[::-1]

# # Ca
# y2 = data['Ca'].values
# y2_upper = []
# y2_lower = []
# for i in y2:
#     y2_upper.append(i+1)
#     y2_lower.append(i-1)
# y2_lower = y2_lower[::-1]

# # Ca+NCX+Na
# y3 = data['Ca+NCX+Na'].values
# y3_upper = []
# y3_lower = []
# for i in y3:
#     y3_upper.append(i+1)
#     y3_lower.append(i-1)
# y3_lower = y3_lower[::-1]

# # 40NCX
# y4 = data['40NCX'].values
# y4_upper = []
# y4_lower = []
# for i in y4:
#     y4_upper.append(i+1)
#     y4_lower.append(i-1)
# y4_lower = y4_lower[::-1]

# # Ca
# y5 = data['Ca'].values
# y5_upper = []
# y5_lower = []
# for i in y5:
#     y5_upper.append(i+1)
#     y5_lower.append(i-1)
# y5_lower = y5_lower[::-1]


# fig = go.Figure()

# fig.add_trace(go.Scatter(
#     x=x+x_rev,
#     y=y1_upper+y1_lower,
#     fill='toself',
#     fillcolor='rgba(0,100,80,0.2)',
#     line_color='rgba(255,255,255,0)',
#     showlegend=False,
#     name='Ca2+ signaling & NCX',
# ))
# fig.add_trace(go.Scatter(
#     x=x+x_rev,
#     y=y2_upper+y2_lower,
#     fill='toself',
#     fillcolor='rgba(0,176,246,0.2)',
#     line_color='rgba(255,255,255,0)',
#     name='Ca2+ signaling',
#     showlegend=False,
# ))
# fig.add_trace(go.Scatter(
#     x=x+x_rev,
#     y=y3_upper+y3_lower,
#     fill='toself',
#     fillcolor='rgba(240,52,52,0.2)',
#     line_color='rgba(255,255,255,0)',
#     showlegend=False,
#     name='Ca2+ sign., NCX & Na+ </>sign.',
# ))
# fig.add_trace(go.Scatter(
#     x=x+x_rev,
#     y=y4_upper+y4_lower,
#     fill='toself',
#     fillcolor='rgba(255,255,0,0.2)',
#     line_color='rgba(255,255,255,0)',
#     showlegend=False,
#     name='40NCX',
# ))
# fig.add_trace(go.Scatter(
#     x=x, y=y1,
#     line_color='rgb(0,100,80)',
#     name='Ca2+ signaling & NCX',
# ))
# fig.add_trace(go.Scatter(
#     x=x, y=y2,
#     line_color='rgb(0,176,246)',
#     name='Ca2+ signaling',
# ))
# fig.add_trace(go.Scatter(
#     x=x, y=y3,
#     line_color='rgb(240,52,52)',
#     name='Ca2+ sign., NCX & Na+ sign.',
# ))
# fig.add_trace(go.Scatter(
#     x=x, y=y4,
#     line_color='rgb(255,255,0)',
#     name='40NCX',
# ))
# fig.add_trace(go.Scatter(
#     x=x, y=y5,
#     line_color='rgb(0,0,0)',
#     name='Ca',
# ))

# Edit the layout
# fig.update_traces(mode='lines+markers', marker_size=10)
# fig.update_layout( xaxis_title='<b>Range (#Cells)<b>',
#                    yaxis_title='<b>Gain (dB)<b>',
#                    yaxis_zeroline=False,
#                    xaxis_zeroline=False,
#                    font=dict(size=16),                   
#                    legend=dict(x=.5, y=1, bgcolor="white"),
#                    paper_bgcolor='rgba(0,0,0,0)',plot_bgcolor='rgba(0,0,0,0)')
# fig.update_yaxes(showline=True, linewidth=2, linecolor='black', showgrid=True, gridwidth=1, gridcolor='lightgray')
# fig.update_xaxes(showline=True, linewidth=2, linecolor='black', showgrid=True, gridwidth=1, gridcolor='lightgray')

# fig.show()

# fig = go.Figure(data=data, layout=layout)
# py.plot(fig, filename='delay_chart.html')

#************************ PLOTLY ************************


# data = pd.read_csv('erros.csv')
# data.head()

# E1 = data['ALPHA_MLP'].values
# E2 = data['ETA_MLP'].values
# E3 = data['ETA_ALPHA_MLP'].values
# E4 = data['NORMAL_MLP'].values
# E5 = data['MLP_ONLINE'].values
# E6 = data['RPROP'].values
# E7 = data['RBF_BP'].values
# E8 = data['RBF_BATCH'].values
# X = np.array(list(zip(E1/max(E1), E2/max(E2), E3/max(E3), E4/max(E4), E5/max(E5), E6/max(E6), E7, E8)), dtype=np.float32)


# epocas = np.linspace(0, X.shape[0], X.shape[0])
# legendas = ['MLP com Alpha', 'MLP com Eta variável', 'MLP com Eta variável e Alpha', 'MLP Original', 'MLP Online', 'RPROP', 'RBF Original', 'RBF Batch']
# estilos_linha = ['-', '-', '-', '--', '-.', '-', '-', '-']

# fig, ax = plt.subplots()
# """
# for i in range(X.shape[-1]):
#     ax.semilogy(epocas, X[:, i], label=legendas[i], linestyle=estilos_linha[i])
# """
# ax.semilogy(epocas, X[:, 6])

# ax.set(xlabel='Épocas de Treinamento', ylabel='Eav')
# ax.grid()
# ax.legend(loc=1)
# fig.savefig("Erros.png")
# plt.show()
