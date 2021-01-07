import pandas as pd
import os
import matplotlib.pyplot as plt


def get_average_deb(csv_debit):
    # Read the csv file:
    df = pd.read_csv(csv_debit, header=None)
    # computes mean value
    # print(df[1])
    sum = 0
    for i in range(len(df[1])):
        #print(df[1][i])
        sum += df[1][i]
    #val = df[1].values.mean()
    #val = 0
    val = sum/len(df[1])
    # print(val)
    return val


def Test_performances():
    # tuples associates the window size to the last update's time
    x = []
    y = []
    for file in os.walk(os.getcwd()):
        for name in file[2]:
            x = []
            y = []
            df = pd.read_csv(name, header=None)
            for i in range(len(df[1])):
                x.append(df[1][i])  # window size
                y.append(df[2][i])
                plt.plot(y, x)

    # beautify the x-labels
    plt.gcf().autofmt_xdate()

    plt.show()
    return

if __name__ == '__main__':

    for file in os.walk(os.getcwd()):
       for name in file[2]:
           means = get_average_deb(name)
           print(f"{name} => moyenne : {means}")

    # Test_performances()