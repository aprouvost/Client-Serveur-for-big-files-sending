import os
import client3
import time
import filecmp


def run(ip, port, file, client):
    current_dir = os.getcwd()
    path = current_dir + "/../input/" + file
    file_size = os.stat(path).st_size
    started = time.perf_counter()
    os.system(f"./client{client} {ip} {port} {file} 0 >> log")
    terminated = time.perf_counter()
    time_run = terminated - started
    debit = (file_size / time_run) * 0.000001
    time.sleep(1)
    # print(f"client{client}, fichier : ", file, " = > Debit : {:.4f} Mo/s".format(debit * 0.000001))
    return debit


if __name__ == '__main__':
    for cl in [1, 2]:
        for asked_file in ["DSC_0647.jpg", "13mo.tif.zip", "100mo.zip", "1g.csv"]:
            for i in range(10):
                with open("result.csv", "w") as result_file:
                    debit = run(ip="134.214.203.35",
                                port=4545,
                                file=asked_file,
                                client=cl)
                    are_equal = filecmp.cmp(f"./copy_{asked_file}", f"../input/{asked_file}")
                    result_file.write(f"client{cl};{asked_file};{debit};{are_equal} \n")
                    print(f"client{cl};{asked_file};{debit};{are_equal}")
