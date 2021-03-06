import pandas            as pd
import numpy             as np
import plotly.graph_objs as go

### Setting Simulation Variables ###
timeSlotStep       = 0.01
timeSlotBegin      = 0.01
timeSlotEnd        = 0.11

freq               = np.around(np.arange(0.1, 1.1, 0.1), 1                           )
timeSlot           = np.around(np.arange(timeSlotBegin, timeSlotEnd, timeSlotStep), 2)
rxRange            = np.arange(1,7                                                   )

### Setting the analysis ###
# evalMetricName     = 'Channel Capacity (bits)'
# evalMetricName     = 'Gain (dB)'
# evalMetricName     = 'SNR (dB)'
evalMetricName     = 'BER'

# referenceVariable  = 'Time Slot (s)'
referenceVariable      = 'Range'

referenceNumber    = 1

# xAxisVariable      = rxRange                                                # The opposite of referenceVariable
xAxisVariable      = freq

# xAxisName          = 'r (#celulas)'                                         # The same as xAxisVariable
xAxisName          = 'f (Hz)'

                    #   Red        Orange     Yellow     Green      Brown      Blue      Blue Sky    Pink     Green Olive   Black               
lineColor          = ['#FF0000', '#FFA500', '#FFFF00', '#008000', '#8B4513', '#0000FF', '#00BFFF', '#FF1493', '#808000', '#000000']  # 10 colors for Time Slot and Frequency

### Select the file ###
# fileName           = "results_NCX_CP.csv"
# fileName           = "results_NCX_CP_04.csv"
# fileName           = "results_NCX_CP_05.csv"
fileName           = "results_NCX_NKP.csv"

### Creating DataFrame from CSV file ###
results            = pd.read_csv("results/" + fileName                               )

### Processing data ###
resultsMean        = results.groupby(['Freq (Hz)', 'Range', 'Time Slot (s)']).mean()

### Graphs of the evaluation Metric ###
fig = go.Figure()

# for f, c in zip(freq, lineColor):
    
#     evalMetric = resultsMean.groupby(['Freq (Hz)', referenceVariable]).get_group( (f, referenceNumber) )

for ts, c in zip(timeSlot, lineColor):
    
    evalMetric = resultsMean.groupby(['Time Slot (s)', referenceVariable]).get_group( (ts, referenceNumber) )

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
        name       = str(ts) + " s",                                                              # If frequency is the main variable
    ))

fig.update_traces(mode='lines+markers', marker_size=5)

fig.update_layout(xaxis_title  = '<b>' + xAxisName + '<b>',
                yaxis_title    = '<b>' + evalMetricName + '<b>',
                yaxis_zeroline = False,
                xaxis_zeroline = False,
                font           = dict(size = 16),
                paper_bgcolor  = 'rgba(0,0,0,0)',plot_bgcolor = 'rgba(0,0,0,0)')

xAxisVariableMaxValue = max( xAxisVariable )
evalMetricMaxValue    = max( np.asarray( evalMetric[ evalMetricName ] ) )

fig.add_annotation(x  = xAxisVariableMaxValue - xAxisVariableMaxValue / 1.5,                       
            y         = evalMetricMaxValue -  evalMetricMaxValue / 8,
            text      = '<b>r = ' + str(referenceNumber) + ' célula(s)<b>',                              # Depending on the reference variable
            showarrow = False)

fig.update_yaxes(showline=True, linewidth=2, linecolor='black', showgrid=True, gridwidth=1, gridcolor='lightgray')
fig.update_xaxes(showline=True, linewidth=2, linecolor='black', showgrid=True, gridwidth=1, gridcolor='lightgray')

fig.show()