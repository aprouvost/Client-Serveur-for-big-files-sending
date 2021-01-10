import os
import filecmp


def compare_files(file1, file2):
    print("Are " + str(file1) + " And " + str(file2) + " Identical? : ")
    print(filecmp.cmp(file1, file1))
    return filecmp.cmp(file1, file1)


if __name__ == '__main__':
    compare_files("/tmp/PRS/input/1g.iso", "/tmp/PRS/clients/copy_1g.iso")
