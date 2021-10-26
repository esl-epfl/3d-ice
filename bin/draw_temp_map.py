# %%
import re
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import matplotlib.pylab as pl
import matplotlib.colorbar as cbar
import pylab as pl
import numpy as np
import argparse

# dictionary for parsing floorplan
rx_dict = {
    # define the keyworks for searching
    'block_name': re.compile(r'(?P<block_name>.*):\n'),
    'position': re.compile(r'position (?P<position>(\d*\.?\d*), (\d*\.?\d*))'),
    'dimension': re.compile(r'dimension (?P<dimension>(\d*\.?\d*), (\d*\.?\d*))'),
    'power1': re.compile(r'power values (?P<power1>\d*\.?\d*)'),
}
def _parse_line(line):
    """
    Do a regex search against all defined regexes and
    return the key and match result of the first matching regex
    """
    for key, rx in rx_dict.items():
        match = rx.search(line)
        if match:
            return key, match
    # if there are no matches
    return None, None

def get_flp_data(file_path):
    data = []
    with open(file_path) as file:
        line = file.readline()
        while line:
            key, match = _parse_line(line)
            # print(key, match)
            if key == 'block_name':
                block_name = match.group("block_name")
            if key == 'position':
                position_x = match.group(2)
                position_y = match.group(3)
            if key == 'dimension':
                dimension_x = match.group(2)
                dimension_y = match.group(3)
            if key == 'power1':
                power1 = match.group("power1")

                row = {
                    'block_name': block_name,
                    'position_x': position_x,
                    'position_y': position_y,
                    'dimension_x': dimension_x,
                    'dimension_y': dimension_y,
                    'power1': power1,
                }
                data.append(row)

            line = file.readline()

    return data

def get_xyaxis_data_nonuniform(file_path):
    data = np.empty((0,4), float)
    with open(file_path) as file:
        line = file.readline()
        while line:
            x = np.array(line.split())
            x=np.asfarray(x,float)
            data=np.append(data, [x], axis=0)
            line = file.readline()

    return data

def get_temp_data_nonuniform(file_path):
    with open(file_path) as file:
        line = file.readline()
        line = file.readline()
        data = np.array(line.split())
        data=np.asfarray(data,float)

    return data

def get_xyaxis_data_uniform(filex_path,filey_path):
    # read length grid information to data_x
    data_x = np.empty((0,1), float)
    with open(filex_path) as file:
        line = file.readline()
        while line:
            x = np.array(line.split())
            x=np.asfarray(x,float)
            data_x=np.append(data_x, [x], axis=0)
            line = file.readline()
    # read width grid information to data_y
    data_y = np.empty((0,1), float)
    with open(filey_path) as file:
        line = file.readline()
        while line:
            x = np.array(line.split())
            x=np.asfarray(x,float)
            data_y=np.append(data_y, [x], axis=0)
            line = file.readline()
    # build rectangle info each temperature grid, and return it as data
    cell_length = data_x[1] - data_x[0]
    cell_width = data_y[1] - data_y[0]
    data = np.empty((0,4), float)
    for x_i in data_y:
        for y_i in data_x:
            x = np.array([y_i-cell_length/2, x_i-cell_width/2, cell_length, cell_width])
            data=np.append(data, [np.squeeze(x)], axis=0)

    return data

def get_temp_data_uniform(file_path):
    # read temperature data from the temperature map file
    data = np.empty((0,1), float)
    with open(file_path) as file:
        line = file.readline()
        line = file.readline()
        while line:
            x = np.array(line.split())
            x=np.asfarray(x,float)
            data=np.append(data, np.transpose([x]), axis=0)
            line = file.readline()

    return data

def plot_tmap(data_flp, xyaxis, temp,
        color_length, temp_min,
        temp_max, show_flp_name,
        output_file
            ):
    # plot the temperature map along with the floorplan
    if temp_min is None:
        temp_min = np.min(temp)-0.001*np.min(temp)
    if temp_max is None:
        temp_max = np.max(temp)+0.001*np.min(temp)

    normal = pl.Normalize(temp_min, temp_max)
    colors = pl.cm.jet(np.linspace(0,1,color_length))
    
    fig, ax = plt.subplots()
    area_sum = 0
    x_max = 0
    y_max = 0
    for data_i in data_flp:
        x_left = float(data_i['position_x'])/1000
        y_left = float(data_i['position_y'])/1000
        dimension_x = float(data_i['dimension_x'])/1000
        dimension_y = float(data_i['dimension_y'])/1000
        area_sum = area_sum+dimension_x*dimension_y
        if x_left+dimension_x>x_max:
            x_max = x_left+dimension_x
        if y_left+dimension_y>y_max:
            y_max = y_left+dimension_y

        rect1 = patches.Rectangle([x_left, y_left], dimension_x, dimension_y, linewidth=1, facecolor="none", edgecolor='k')
        ax.add_patch(rect1)
        if show_flp_name == "True":
            ax.text(x_left+dimension_x/2, y_left+dimension_y/2, data_i['block_name'],
                horizontalalignment='center',
                verticalalignment='center',)

    for i in range(len(temp)):
        x_left = xyaxis[i,0]/1000
        y_left = xyaxis[i,1]/1000
        dimension_x = xyaxis[i,2]/1000
        dimension_y = xyaxis[i,3]/1000

        color_index = int( (temp[i]-temp_min)/(temp_max-temp_min) * color_length)
        rect1 = patches.Rectangle([x_left, y_left], dimension_x, dimension_y, linewidth=1, alpha=0.5, facecolor=colors[color_index])
        ax.add_patch(rect1)

    cax, _ = cbar.make_axes(ax) 
    cb2 = cbar.ColorbarBase(cax, cmap=pl.cm.jet,norm=normal) 
    cb2.set_label('Temperature (K)')
    # set aspect ratio based on the axis
    ratio = 1.0
    xleft, xright = ax.get_xlim()
    ybottom, ytop = ax.get_ylim()
    ax.set_aspect(abs((xright-xleft)/(ybottom-ytop))*ratio)

    ax.set_xlabel('X (mm)')
    ax.set_ylabel('Y (mm)')
    ax.relim()
    ax.autoscale_view()
    plt.savefig(output_file, format="svg")

    if x_max*y_max != area_sum:
        print("area mismatch. Blocks area:",area_sum, "total area:", x_max*y_max,", please check the floorplan")

# %%
def main():
    parser = argparse.ArgumentParser()

    parser.add_argument('-f', '--flp', nargs='?', type=str, required=True,
    help='input floorplan file')
    parser.add_argument('-t', '--tmap', nargs='?', type=str, required=True,
    help='input temperature map file')
    parser.add_argument('-xy', '--xyaxis', nargs='?', type=str,
    help='[non-uniform mode] input xyaxis file')
    parser.add_argument('-x', '--xaxis', nargs='?', type=str,
    help='[uniform mode] input xaxis file')
    parser.add_argument('-y', '--yaxis', nargs='?', type=str,
    help='[uniform mode] input yaxis file')
    parser.add_argument('-o', '--output', nargs='?', type=str, default='test.svg',
    help='output file, it is test.svg if not defined')
    parser.add_argument('-tmin', '--tempmin', nargs='?', type=float,
    help='min temperature in the temperature map, it adapts to the input temperature if not defined')
    parser.add_argument('-tmax', '--tempmax', nargs='?', type=float,
    help='max temperature in the temperature map, it adapts to the input temperature if not defined')
    parser.add_argument('-fn', '--flpname', nargs='?', type=str, default='True',
    help='show the block name in the output figure, it is True by default')
    parser.add_argument('-c', '--color', nargs='?', type=int, default=100,
    help='The resoluion of the color bar, it is 100 by default')
    args = parser.parse_args()

    flp_path = args.flp
    data_flp = get_flp_data(flp_path)

    if args.xyaxis is not None:
        xyaxis_path = args.xyaxis
        xyaxis = get_xyaxis_data_nonuniform(xyaxis_path)

        tmap_path = args.tmap
        temp = get_temp_data_nonuniform(tmap_path)
    else:

        xaxis_path = args.xaxis
        yaxis_path = args.yaxis
        xyaxis = get_xyaxis_data_uniform(xaxis_path, yaxis_path)

        tmap_path = args.tmap
        temp = get_temp_data_uniform(tmap_path)

    plot_tmap(data_flp, xyaxis, temp, args.color, args.tempmin, args.tempmax, args.flpname, args.output)


if __name__ == '__main__':
    main()
