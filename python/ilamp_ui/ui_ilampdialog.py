# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ilampdialog.ui'
#
# Created by: PyQt5 UI code generator 5.10
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_IlampDialog(object):
    def setupUi(self, IlampDialog):
        IlampDialog.setObjectName("IlampDialog")
        IlampDialog.resize(371, 203)
        IlampDialog.setMinimumSize(QtCore.QSize(371, 203))
        self.gridLayout_2 = QtWidgets.QGridLayout(IlampDialog)
        self.gridLayout_2.setObjectName("gridLayout_2")
        self.gridLayout = QtWidgets.QGridLayout()
        self.gridLayout.setObjectName("gridLayout")
        self.projectNameLabel = QtWidgets.QLabel(IlampDialog)
        self.projectNameLabel.setMinimumSize(QtCore.QSize(71, 21))
        self.projectNameLabel.setMaximumSize(QtCore.QSize(71, 21))
        self.projectNameLabel.setObjectName("projectNameLabel")
        self.gridLayout.addWidget(self.projectNameLabel, 0, 0, 1, 1)
        self.projectNameLineEdit = QtWidgets.QLineEdit(IlampDialog)
        self.projectNameLineEdit.setObjectName("projectNameLineEdit")
        self.gridLayout.addWidget(self.projectNameLineEdit, 0, 1, 1, 1)
        self.projectNameButton = QtWidgets.QPushButton(IlampDialog)
        self.projectNameButton.setMinimumSize(QtCore.QSize(31, 23))
        self.projectNameButton.setMaximumSize(QtCore.QSize(31, 23))
        self.projectNameButton.setObjectName("projectNameButton")
        self.gridLayout.addWidget(self.projectNameButton, 0, 2, 1, 1)
        self.inputFolderLabel = QtWidgets.QLabel(IlampDialog)
        self.inputFolderLabel.setMinimumSize(QtCore.QSize(71, 21))
        self.inputFolderLabel.setMaximumSize(QtCore.QSize(71, 21))
        self.inputFolderLabel.setObjectName("inputFolderLabel")
        self.gridLayout.addWidget(self.inputFolderLabel, 1, 0, 1, 1)
        self.inputFolderLineEdit = QtWidgets.QLineEdit(IlampDialog)
        self.inputFolderLineEdit.setObjectName("inputFolderLineEdit")
        self.gridLayout.addWidget(self.inputFolderLineEdit, 1, 1, 1, 1)
        self.inputFolderButton = QtWidgets.QPushButton(IlampDialog)
        self.inputFolderButton.setMinimumSize(QtCore.QSize(31, 23))
        self.inputFolderButton.setMaximumSize(QtCore.QSize(31, 23))
        self.inputFolderButton.setObjectName("inputFolderButton")
        self.gridLayout.addWidget(self.inputFolderButton, 1, 2, 1, 1)
        self.filepath2dLabel = QtWidgets.QLabel(IlampDialog)
        self.filepath2dLabel.setMinimumSize(QtCore.QSize(71, 21))
        self.filepath2dLabel.setMaximumSize(QtCore.QSize(71, 21))
        self.filepath2dLabel.setObjectName("filepath2dLabel")
        self.gridLayout.addWidget(self.filepath2dLabel, 2, 0, 1, 1)
        self.filepath2dLineEdit = QtWidgets.QLineEdit(IlampDialog)
        self.filepath2dLineEdit.setObjectName("filepath2dLineEdit")
        self.gridLayout.addWidget(self.filepath2dLineEdit, 2, 1, 1, 1)
        self.filepath2dButton = QtWidgets.QPushButton(IlampDialog)
        self.filepath2dButton.setMinimumSize(QtCore.QSize(31, 23))
        self.filepath2dButton.setMaximumSize(QtCore.QSize(31, 23))
        self.filepath2dButton.setObjectName("filepath2dButton")
        self.gridLayout.addWidget(self.filepath2dButton, 2, 2, 1, 1)
        self.filepathndLabel = QtWidgets.QLabel(IlampDialog)
        self.filepathndLabel.setMinimumSize(QtCore.QSize(71, 21))
        self.filepathndLabel.setMaximumSize(QtCore.QSize(71, 21))
        self.filepathndLabel.setObjectName("filepathndLabel")
        self.gridLayout.addWidget(self.filepathndLabel, 3, 0, 1, 1)
        self.filepathndLineEdit = QtWidgets.QLineEdit(IlampDialog)
        self.filepathndLineEdit.setObjectName("filepathndLineEdit")
        self.gridLayout.addWidget(self.filepathndLineEdit, 3, 1, 1, 1)
        self.filepathndButton = QtWidgets.QPushButton(IlampDialog)
        self.filepathndButton.setMinimumSize(QtCore.QSize(31, 23))
        self.filepathndButton.setMaximumSize(QtCore.QSize(31, 23))
        self.filepathndButton.setObjectName("filepathndButton")
        self.gridLayout.addWidget(self.filepathndButton, 3, 2, 1, 1)
        self.outputFolderLabel = QtWidgets.QLabel(IlampDialog)
        self.outputFolderLabel.setMinimumSize(QtCore.QSize(71, 21))
        self.outputFolderLabel.setMaximumSize(QtCore.QSize(71, 21))
        self.outputFolderLabel.setObjectName("outputFolderLabel")
        self.gridLayout.addWidget(self.outputFolderLabel, 4, 0, 1, 1)
        self.outputFolderLineEdit = QtWidgets.QLineEdit(IlampDialog)
        self.outputFolderLineEdit.setObjectName("outputFolderLineEdit")
        self.gridLayout.addWidget(self.outputFolderLineEdit, 4, 1, 1, 1)
        self.outputFolderButton = QtWidgets.QPushButton(IlampDialog)
        self.outputFolderButton.setMinimumSize(QtCore.QSize(31, 23))
        self.outputFolderButton.setMaximumSize(QtCore.QSize(31, 23))
        self.outputFolderButton.setObjectName("outputFolderButton")
        self.gridLayout.addWidget(self.outputFolderButton, 4, 2, 1, 1)
        self.gridLayout_2.addLayout(self.gridLayout, 0, 0, 1, 1)
        self.buttonBox = QtWidgets.QDialogButtonBox(IlampDialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtWidgets.QDialogButtonBox.Cancel|QtWidgets.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.gridLayout_2.addWidget(self.buttonBox, 1, 0, 1, 1)

        self.retranslateUi(IlampDialog)
        self.buttonBox.accepted.connect(IlampDialog.accept)
        self.buttonBox.rejected.connect(IlampDialog.reject)
        self.projectNameButton.clicked.connect(IlampDialog.onProjectNameClicked)
        self.inputFolderButton.clicked.connect(IlampDialog.onInputFolderClicked)
        self.outputFolderButton.clicked.connect(IlampDialog.onOutputFolderClicked)
        QtCore.QMetaObject.connectSlotsByName(IlampDialog)

    def retranslateUi(self, IlampDialog):
        _translate = QtCore.QCoreApplication.translate
        IlampDialog.setWindowTitle(_translate("IlampDialog", "ILamp Project Dialog"))
        self.projectNameLabel.setText(_translate("IlampDialog", "Project Name:"))
        self.projectNameButton.setText(_translate("IlampDialog", "..."))
        self.inputFolderLabel.setText(_translate("IlampDialog", "Input Folder:"))
        self.inputFolderButton.setText(_translate("IlampDialog", "..."))
        self.filepath2dLabel.setText(_translate("IlampDialog", "File Path 2d:"))
        self.filepath2dButton.setText(_translate("IlampDialog", "..."))
        self.filepathndLabel.setText(_translate("IlampDialog", "File Path nd:"))
        self.filepathndButton.setText(_translate("IlampDialog", "..."))
        self.outputFolderLabel.setText(_translate("IlampDialog", "Ouput Folder:"))
        self.outputFolderButton.setText(_translate("IlampDialog", "..."))


if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    IlampDialog = QtWidgets.QDialog()
    ui = Ui_IlampDialog()
    ui.setupUi(IlampDialog)
    IlampDialog.show()
    sys.exit(app.exec_())

