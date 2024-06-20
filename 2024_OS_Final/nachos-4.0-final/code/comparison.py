# code from: https://www.geeksforgeeks.org/how-to-compare-two-text-files-in-python/
# reading files
f1 = open("./output.txt", "r", encoding="utf-8")
f2 = open("./output_my.txt", "r", encoding="utf-8")

f1_data = f1.readlines()
f2_data = f2.readlines()


def remove_brackets(data):
    new_data = []
    for line in data:
        new_data.append(line.replace("[", "").replace("]", "").replace(" ", ""))
    return new_data


f1_data = remove_brackets(f1_data)
f2_data = remove_brackets(f2_data)

data = list(zip(f1_data, f2_data))
# print(data)
# input()

i = 0

for line in data:
    i += 1
    if line[0] == line[1]:
        continue
        print("Line ", i, ": IDENTICAL")
    else:
        print("Line ", i, ":")
        print("\tFile 1:", line[0], end="")
        print("\tFile 2:", line[1], end="")

# for line1 in f1_data:
#     i += 1

#     for line2 in f2_data:

#         # matching line1 from both files
#         if line1 == line2:
#             # print IDENTICAL if similar
#             print("Line ", i, ": IDENTICAL")
#         else:
#             print("Line ", i, ":")
#             # else print that line from both files
#             print("\tFile 1:", line1, end="")
#             print("\tFile 2:", line2, end="")
#         break

# closing files
f1.close()
f2.close()
