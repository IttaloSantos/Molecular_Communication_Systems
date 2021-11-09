from numpy.core.fromnumeric import shape
import pandas            as pd
import numpy             as np
import plotly.graph_objs as go
import matplotlib.pyplot as plt
from scipy import interpolate

### Setting Simulation Variables ###
timeSlotStep       = 1e-4
timeSlotBegin      = 5e-4
timeSlotEnd        = 1.1e-3
interSamplesNumber = 12

freq               = np.around(np.arange(0.1, 1.1, 0.1), 1                           )
timeSlot           = 5e-3 # 5e-4,6e-4,7e-4,8e-4,9e-4,1e-3,2e-3,3e-3,4e-3,5e-3
rxRange            = np.arange(1,7                                                   )

### Setting the analysis ###
# evalMetricName     = 'Channel Capacity (bits)'
# evalMetricName     = 'Gain (dB)'
evalMetricName     = 'SNR (dB)'
# evalMetricName     = 'BER'

# referenceVariable  = 'Time Slot (s)'
referenceVariable  = 'Range'                                              # Variable that will be showed in the text

referenceNumber    = 1                                                    # The value for the referenceVariable that will be considered on the results

# xAxisVariable      = rxRange                                              # The opposite of referenceVariable
xAxisVariable      = freq
# xAxisVariable      = timeSlot

# xAxisName          = 'r (#celulas)'                                       # The same as xAxisVariable
xAxisName          = 'f (Hz)'
# xAxisName          = 'Tb (s)'

                    #   Black     Red      Blue     Green                   
lineColor          = ['#FF0000', '#0000FF', '#008000'] #'#000000',

### Select the file ###
fileName           = ["Na_NCX.csv","NCX_NKP_CP.csv", "Na_NCX_NKP_CP.csv"] #"C.csv",
tracesName         = ["c_1, c_2 e c_3", "c_1, c_3, c_4 e c_5", "Todas fechadas"] # "Apenas c_1", 
i                  = 0
### Graphs of the evaluation Metric ###
fig = go.Figure()
figs, ax = plt.subplots()

for file, c in zip(fileName, lineColor):

    ### Creating DataFrame from CSV file ###
    results     = pd.read_csv("results/" + file)

    ### Processing data ###
    resultsMean = results.groupby(['Freq (Hz)', 'Range', 'Time Slot (s)']).mean()
    
    evalMetric = resultsMean.groupby(['Time Slot (s)', referenceVariable]).get_group( (timeSlot, referenceNumber) )

    y_1 = np.array(evalMetric[ evalMetricName ])

    cubic_interp = interpolate.interp1d(xAxisVariable, y_1)
    xnew         = np.linspace(min(xAxisVariable), max(xAxisVariable), interSamplesNumber)

    fig.add_trace(go.Scatter(
        x          = xAxisVariable, 
        y          = y_1,# y_1, cubic_interp(xnew)
        line_color = c,
        name       = tracesName[i],                                                              # If frequency is the main variable
    ))
    i += 1

fig.update_traces(mode='lines+markers', marker_size=5)

fig.update_layout(xaxis_title  = '<b>' + xAxisName + '<b>',
                yaxis_title    = '<b>' + evalMetricName + '<b>', # 'Ganho (dB)', 'Capacidade do canal (bps)'
                yaxis_zeroline = False,
                xaxis_zeroline = False,
                font           = dict(size = 15),
                paper_bgcolor  = 'rgba(0,0,0,0)',plot_bgcolor = 'rgba(0,0,0,0)')

xAxisVariableMaxValue = max( xAxisVariable )
evalMetricMaxValue    = max( np.asarray( evalMetric[ evalMetricName ] ) )

# fig.add_annotation(x  = xAxisVariableMaxValue - 0.5*xAxisVariableMaxValue,# / 1.5,                       
#             y         = evalMetricMaxValue,# -  evalMetricMaxValue / 8,
#             text      = '<b>r = ' + str(referenceNumber) + ' Célula<b>',                              # Depending on the reference variable
#             showarrow = False)

fig.update_yaxes(showline=True, linewidth=2, linecolor='black', showgrid=True, gridwidth=1, gridcolor='lightgray')
fig.update_xaxes(showline=True, linewidth=2, linecolor='black', showgrid=True, gridwidth=1, gridcolor='lightgray')

fig.show()