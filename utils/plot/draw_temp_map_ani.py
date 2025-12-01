# %%
import re
import matplotlib
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import matplotlib.pylab as pl
import matplotlib.colorbar as cbar
from matplotlib.animation import FuncAnimation
from matplotlib.collections import PatchCollection
from matplotlib.widgets import Slider
import pylab as pl
import numpy as np
import argparse

# dic for parsing floorplan
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

def get_temp_data_nonuniform(file_path, grid_num):
    data = np.empty((0,grid_num), float)
    with open(file_path) as file:
        line = file.readline()
        line = file.readline()
        while line:
            x = np.array(line.split())
            x=np.asfarray(x,float)
            data=np.append(data, [x], axis=0)
            line = file.readline()
            line = file.readline()

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

def plot_tmap_ani(data_flp, xyaxis, temp, \
        color_length, temp_min, \
        temp_max, show_flp_name, \
        output_file, timestep, frame_count
            ):
    # plot the temperature map along with the floorplan
    if temp_min is None:
        temp_min = np.min(temp)-0.001*np.min(temp)
    if temp_max is None:
        temp_max = np.max(temp)+0.001*np.max(temp)

    norm = pl.Normalize(temp_min, temp_max)
    cmap=plt.cm.jet
    plt.rcParams['font.family'] = 'monospace'
    fig, ax = plt.subplots(dpi = 300)
    
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
    cax, _ = cbar.make_axes(ax) 
    cb2 = cbar.ColorbarBase(cax, cmap=pl.cm.jet,norm=norm) 
    cb2.set_label('Temperature (K)')
    ratio = 1.0
    xleft, xright = ax.get_xlim()
    ybottom, ytop = ax.get_ylim()
    ax.set_aspect(abs((xright-xleft)/(ybottom-ytop))*ratio)
    ax.set_ylabel('Dimension (mm)')
    ax.autoscale_view()

    xyaxis = xyaxis/1000 #change to mm
    str_len = 30
    
    temp_all = temp
    def animate(t_i):
            
        print("t_i:",t_i)
        temp = temp_all[t_i,:]
        pat = []
        color_list = []
        for i in range(len(temp)):
            color_list = np.append(color_list,temp[i])
            rect=patches.Rectangle((xyaxis[i,0],xyaxis[i,1]),xyaxis[i,2],xyaxis[i,3])
            pat.append(rect)
        col = PatchCollection(pat, cmap=cmap, norm=norm)
        col.set_array(color_list)
        ax.add_collection(col)
        cur_slot = int(str_len*(t_i+1)/frame_count)
        string_val = "|"+ "-" * cur_slot + ">" + " " *(str_len-cur_slot) + "|" + "t=" + f'{t_i*timestep:.2f}' + "s"
        ax.set_xlabel(string_val, loc='left')

    ani = FuncAnimation(fig, animate, save_count=frame_count)
    writergif = matplotlib.animation.PillowWriter(fps=1/timestep)
    ani.save(output_file, writer=writergif)
    print("x_max:",x_max)
    print("y_max:",y_max)
    if x_max*y_max != area_sum:
        print("area mismatch. Blocks area:",area_sum, "total area:", x_max*y_max,", please check the floorplan")

# %%
def main():
    parser = argparse.ArgumentParser()

    parser.add_argument('-f', '--flp', nargs='?', type=str, required=True,
    help='input floorplan file')
    parser.add_argument('-t', '--tmap', nargs='?', type=str, required=True,
    help='input temperature map file')
    parser.add_argument('-ts', '--timestep', nargs='?', type=float, required=True,
    help='time step of the animation')
    parser.add_argument('-xy', '--xyaxis', nargs='?', type=str,
    help='[non-uniform mode] input xyaxis file')
    parser.add_argument('-x', '--xaxis', nargs='?', type=str,
    help='[uniform mode] input xaxis file')
    parser.add_argument('-y', '--yaxis', nargs='?', type=str,
    help='[uniform mode] input yaxis file')
    parser.add_argument('-o', '--output', nargs='?', type=str, default='animation.gif',
    help='output file, it is animation.gif if not defined')
    parser.add_argument('-tmin', '--tempmin', nargs='?', type=float,
    help='min temperature in the temperature map, it adapts to the input temperature if not defined')
    parser.add_argument('-tmax', '--tempmax', nargs='?', type=float,
    help='max temperature in the temperature map, it adapts to the input temperature if not defined')
    parser.add_argument('-fn', '--flpname', nargs='?', type=str, default='True',
    help='show the block name in the output figure, it is True by default')
    parser.add_argument('-c', '--color', nargs='?', type=int, default=100,
    help='The resoluion of the color bar, it is 100 by default')
    parser.add_argument('-fc', '--framecount', nargs='?', type=int,
    help='The framecount animation contains, by default is all frames the temperature map file contains')
    args = parser.parse_args()

    flp_path = args.flp
    data_flp = get_flp_data(flp_path)

    if args.xyaxis is not None:
        xyaxis_path = args.xyaxis
        xyaxis = get_xyaxis_data_nonuniform(xyaxis_path)

        tmap_path = args.tmap
        temp = get_temp_data_nonuniform(tmap_path,int(np.size(xyaxis)/4))
    else:

        xaxis_path = args.xaxis
        yaxis_path = args.yaxis
        xyaxis = get_xyaxis_data_uniform(xaxis_path, yaxis_path)

        tmap_path = args.tmap
        temp = get_temp_data_uniform(tmap_path)

    if args.framecount is not None:
        framecount = args.framecount
    else:
        framecount = np.shape(temp)[0]

    plot_tmap_ani(data_flp, xyaxis, temp, args.color, args.tempmin, args.tempmax, args.flpname, args.output, args.timestep, framecount)


if __name__ == '__main__':
    main()