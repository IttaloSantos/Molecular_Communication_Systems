import pandas            as pd
import numpy             as np
import plotly.graph_objs as go
import matplotlib.pyplot as plt

### Setting Simulation Variables ###
timeSlotStep       = 1e-4
timeSlotBegin      = 5e-4
timeSlotEnd        = 1.1e-3

freq               = np.around(np.arange(0.1, 1.1, 0.1), 1                           )
timeSlot           = [5e-4,6e-4,7e-4,8e-4,9e-4,1e-3,2e-3,3e-3,4e-3,5e-3] #np.around(np.arange(timeSlotBegin, timeSlotEnd, timeSlotStep), 4)
rxRange            = np.arange(1,7                                                   )

### Setting the analysis ###
# evalMetricName     = 'Channel Capacity (bits)'
evalMetricName     = 'Gain (dB)'
# evalMetricName     = 'SNR (dB)'
# evalMetricName     = 'BER'

referenceVariable  = 'Time Slot (s)'
# referenceVariable  = 'Range'                                              # Variable that will be showed in the text

referenceNumber    = 2e-3                                                    # The value for the referenceVariable that will be considered on the results

xAxisVariable      = rxRange                                              # The opposite of referenceVariable
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

for f, c in zip(freq, lineColor):
    
    evalMetric = resultsMean.groupby(['Freq (Hz)', referenceVariable]).get_group( (f, referenceNumber) )

    # for ts, c in zip(timeSlot, lineColor):
        
    #     evalMetric = resultsMean.groupby(['Time Slot (s)', referenceVariable]).get_group( (ts, referenceNumber) )

    # fig.add_trace(go.Scatter(
    #     x=x+x_rev,
    #     y=y1_upper+y1_lower,
    #     fill='toself',
    #     fillcolor='rgba(0,0,255,0.2)',
    #     line_color='rgba(255,255,255,0)',
    #     showlegend=False,
    #     name='tx: [Ca2+]i',
    # ))

    fig.add_trace(go.Scatter(
        x          = xAxisVariable, 
        y          = np.asarray( evalMetric[ evalMetricName ] ),
        line_color = c,
        name       = str(f) + " Hz",                                                              # If frequency is the main variable
    ))

    # ax.semilogy(np.asarray( evalMetric['SNR (dB)'] ), np.asarray( evalMetric[evalMetricName] ) ) # , label=legendas[i], linestyle=estilos_linha[i]
#     ax.semilogx(timeSlot, np.asarray( evalMetric[evalMetricName] ) )

# # ax.set(xlabel='SNR (dB)', ylabel='BER')
# ax.set(xlabel='Tb (s)', ylabel='BER')
# ax.grid()
# ax.legend(loc=1)
# plt.savefig("ber_c1_TbxF.png")
# plt.show()

fig.update_traces(mode='lines+markers', marker_size=5)

fig.update_layout(xaxis_title  = '<b>' + xAxisName + '<b>',
                yaxis_title    = '<b>' + evalMetricName + '<b>',
                yaxis_zeroline = False,
                xaxis_zeroline = False,
                font           = dict(size = 16),
                paper_bgcolor  = 'rgba(0,0,0,0)',plot_bgcolor = 'rgba(0,0,0,0)')

xAxisVariableMaxValue = max( xAxisVariable )
evalMetricMaxValue    = max( np.asarray( evalMetric[ evalMetricName ] ) )

fig.add_annotation(x  = xAxisVariableMaxValue - 0.5*xAxisVariableMaxValue,# / 1.5,                       
            y         = evalMetricMaxValue,# -  evalMetricMaxValue / 8,
            text      = '<b>Tb = ' + str(referenceNumber) + ' s<b>',                              # Depending on the reference variable
            showarrow = False)

fig.update_yaxes(showline=True, linewidth=2, linecolor='black', showgrid=True, gridwidth=1, gridcolor='lightgray')
fig.update_xaxes(showline=True, linewidth=2, linecolor='black', showgrid=True, gridwidth=1, gridcolor='lightgray')

fig.show()