
import os
import filecmp


def compare_files(file1, file2):
    print("Are " + str(file1) + " And " + str(file2) + " Identical? : ")
    print(filecmp.cmp(os.mkdir(file1), os.mkdir(file1)))
    return filecmp.cmp(os.mkdir(file1), os.mkdir(file1))
