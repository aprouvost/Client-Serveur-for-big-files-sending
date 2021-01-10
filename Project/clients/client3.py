import multiprocessing
import os
import shutil


def client(param):
    # print(os.getcwd())
    # os.mkdir(f"client3_{param}")
    # print(os.getcwd())
    # shutil.copyfile("client1", f"client3_{param}/client1")
    os.chdir(f"client3_{param}")
    print(os.getcwd())
    # os.chmod("client1", 0o755)
    os.system(f"./client1 134.214.202.238 1234 DSC_0647.jpg >> client3_{param}.log")
    return


if __name__ == "__main__":
    pool = [multiprocessing.Process(target=client, args=(i,)) for i in range(5)]

    for th in pool:
        th.start()

    for th in pool:
        th.join()
    # client(0)