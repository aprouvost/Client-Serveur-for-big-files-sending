from statistics import mean

with open("../perf/result.csv", "r") as file:
    data = file.read()
    line = data.split('\n')
    debit_value = []
    for elem in line:
        field = elem.split(';')
        try:
            debit_value.append(float(field[2]))
        except IndexError:
            pass
    print(mean(debit_value))
