import os

# Get all files in ../src and ../include
paths = []
for root, dirs, files in os.walk("../src"):
    for file in files:
        if file.endswith(".cpp"):
            paths.append(f"{os.path.join(root, file)}")

for root, dirs, files in os.walk("../include"):
    for file in files:
        if file.endswith(".h"):
            paths.append(f"{os.path.join(root, file)}")

# LaTex output
for path in paths:
    print("\\subsubsection {\\textit{" + path.replace("../", "").replace("_", "\\_") + "}}")
    print("\\inputminted[linenos]{c++}{" + path + "}")
    print("\\pagebreak")
