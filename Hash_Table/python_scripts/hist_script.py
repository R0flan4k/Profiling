from plotly import graph_objects as go
import plotly.express as px

for i in range(4):
    data = []

    with open("./Hash_Table_1st_opt/python_scripts/hist_data" + str(i) + ".data") as f:
        for line in f:
            data.append([str(x) for x in line.split()])
                    
    x = data[0]
    y = data[1]
    label = "load factor = " + str(data[2][0]) + "          dispersion = " + str(data[3][0])
    fig = go.Figure()
                    
    fig.add_trace(go.Histogram(histfunc="sum", y=y, x=x, name="sum"))
    fig.update_layout(xaxis_title_text = label, title_text = "Hash Table load: hash function " + str(i))

                    
    fig.write_image("./Hash_Table_1st_opt/python_scripts/hist" + str(i) + ".png")
    fig.write_html("./Hash_Table_1st_opt/python_scripts/hist" + str(i) + ".html")
