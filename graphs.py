import pandas            as pd
import numpy             as np
import plotly.graph_objs as go
import matplotlib.pyplot as plt

### Setting Simulation Variables ###
timeSlotStep       = 0.001
timeSlotBegin      = 0.001
timeSlotEnd        = 0.011
evalMetricMaxValue = 0

freq               = np.around(np.arange(0.1, 1.1, 0.1), 1                           )
timeSlot           = np.around(np.arange(timeSlotBegin, timeSlotEnd, timeSlotStep), 2)
rxRange            = np.arange(1,7                                                   )

### Setting the analysis ###
evalMetricName     = 'Channel Capacity (bits)'
# evalMetricName     = 'Gain (dB)'
# evalMetricName     = 'SNR (dB)'
# evalMetricName     = 'BER'

# evalMetricNamePt   = evalMetricName
evalMetricNamePt   = 'Capacidade do Canal (bits)'
# evalMetricNamePt   = 'Ganho (dB)'

referenceVariable  = 'Time Slot (s)'                                    # When reference number is Tb, change the axisVariable (for)
# referenceVariable  = 'Range'                                              # Variable that will be showed in the text

referenceNumber    = 0.008                                                    # The value for the referenceVariable that will be considered on the results

xAxisVariable      = rxRange                                              # Variable which is out of getgroup
# xAxisVariable      = freq
# xAxisVariable      = timeSlot

xAxisName          = 'r (#celulas)'                                       # The same as xAxisVariable
# xAxisName          = 'f (Hz)'
# xAxisName          = 'Tb (s)'

                    #   Red        Orange     Yellow     Green      Brown      Blue      Blue Sky    Pink     Green Olive   Black               
lineColor          = ['#FF0000', '#FFA500', '#FFFF00', '#008000', '#8B4513', '#0000FF', '#00BFFF', '#FF1493', '#808000', '#000000']  # 10 colors for Time Slot and Frequency

### Select the file ###
fileName           = "C.csv"
# fileName           = "NCX_Na.csv"
# fileName           = "NCX_Na_NKP_CP.csv"
# fileName           = "NCX_NKP_CP.csv"

### Creating DataFrame from CSV file ###
results            = pd.read_csv("results/" + fileName                               )

### Processing data ###
resultsMean        = results.groupby(['Freq (Hz)', 'Range', 'Time Slot (s)']).mean()

### Graphs of the evaluation Metric ###
fig = go.Figure()
figs, ax = plt.subplots()

for f, c in zip(freq, lineColor): # Variable of the legend
    
    evalMetric = resultsMean.groupby(['Freq (Hz)', referenceVariable]).get_group( (f, referenceNumber) )

    # for ts, c in zip(timeSlot, lineColor):
        
    #     evalMetric = resultsMean.groupby(['Time Slot (s)', referenceVariable]).get_group( (ts, referenceNumber) )

    if referenceVariable != 'Range':
        fig.add_trace(go.Scatter(
            x          = xAxisVariable, 
            y          = np.asarray( evalMetric[ evalMetricName ] ),
            line_color = c,
            name       = str(f) + " Hz",
        ))

        text = '<b>Tb = ' + str(referenceNumber) + ' s<b>'

    else:
        fig.add_trace(go.Scatter(
            x          = xAxisVariable, 
            y          = np.asarray( evalMetric[ evalMetricName ] ),
            line_color = c,
            name       = str(ts) + " s",
        ))

        text = '<b>r = ' + str(referenceNumber) + ' célula(s)<b>'

    evalMetricMaxValue = max(evalMetricMaxValue, max( np.asarray( evalMetric[ evalMetricName ] ) ) )

    # ax.semilogy(np.asarray( evalMetric['SNR (dB)'] ), np.asarray( evalMetric[evalMetricName] ) ) # , label=legendas[i], linestyle=estilos_linha[i]

# ax.set(xlabel='SNR (dB)', ylabel='BER')
# ax.grid()
# ax.legend(loc=1)
# fig.savefig("/home/ittalo/Documentos/Projeto Comunicação Molecular/Imagens/SISO/SNRxBER.png")
# plt.show()

xAxisVariableMaxValue = max(xAxisVariable)

fig.update_traces(mode='lines+markers', marker_size=5)

fig.update_layout(xaxis_title  = '<b>' + xAxisName + '<b>',
                yaxis_title    = '<b>' + evalMetricNamePt + '<b>',
                yaxis_zeroline = False,
                xaxis_zeroline = False,
                font           = dict(size = 16),
                paper_bgcolor  = 'rgba(0,0,0,0)',plot_bgcolor = 'rgba(0,0,0,0)')

fig.add_annotation(x  = xAxisVariableMaxValue / 2,                       
            y         = 1.001 * evalMetricMaxValue,
            text      = text,                              
            showarrow = False)

fig.update_yaxes(showline=True, linewidth=2, linecolor='black', showgrid=True, gridwidth=1, gridcolor='lightgray')
fig.update_xaxes(showline=True, linewidth=2, linecolor='black', showgrid=True, gridwidth=1, gridcolor='lightgray')

fig.show()
