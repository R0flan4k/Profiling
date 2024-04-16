from plotly import graph_objects as go

x = ["1", "2", "3", "4", "5"]
y = ["2","10","2","10","2"]

fig = go.Figure()
fig.add_trace(go.Histogram(histfunc="sum", y=y, x=x, name="sum"))

fig.show()