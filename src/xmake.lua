target("NemesisUnlimitedBehaviorEngine")
    add_files("**.cpp")
    add_files("../resources/*.rc", "../resources/*.qrc")
    add_frameworks("QtGui", "QtWidgets") -- "QtCore" is default
    add_includedirs("../include", "../external","../Python3_8_10_x86-windows/Python38-32/include")
    add_linkdirs("../Python3_8_10_x86-windows/Python38-32/libs")
    add_packages("boost", "qt5")
    add_rules("qt.application") -- ref: https://tboox.org/2018/05/30/support-qt/#:~:text=Quick%20Application%20Program
