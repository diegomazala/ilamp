#from time import time
import numpy as np
#from sklearn import datasets
#from sklearn.manifold import MDS,TSNE
#from scipy.stats.mstats import zscore
#import matplotlib.pyplot as plt
import sys


from lamp import Lamp

input_nd_file = "iris.data"
output_2d_file = "iris_out.2d"

# Check if we have input arguments from command line
if (len(sys.argv) > 2):
    output_2d_file = str(sys.argv[2])
if (len(sys.argv) > 1):
    input_nd_file = str(sys.argv[1])

print("- Input  : ", input_nd_file )
print("- Output : ", output_2d_file )

#iris = datasets.load_iris()
#x = iris.data
#y = iris.target


# including labels as the last column
#data = np.hstack((x,y.reshape(y.shape[0],1)))

print("- Loading input file... ")
data = np.loadtxt(input_nd_file)


##### using Lamp
print("- Computing... ")
#ts = time()
lamp_proj = Lamp(Xdata = data, label=True)
data_proj = lamp_proj.fit()
#print('Took {}s'.format(time() - ts))

#print("- Plotting... ")
#plt.scatter(data_proj[:,0],data_proj[:,1])
#plt.scatter(data_proj[:,0],data_proj[:,1],c=data[:,-1])
#plt.show()


print("- Saving result: ", output_2d_file)
#np.savetxt(output_2d_file, matrix_2d, fmt='%10.5f', delimiter=",")
np.savetxt(output_2d_file, data_proj, fmt='%.5f')

print("- Finished! ")
