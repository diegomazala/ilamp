# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ilampdialog.ui'
#
# Created by: PyQt5 UI code generator 5.10
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_ILampDialog(object):
    def setupUi(self, ILampDialog):
        ILampDialog.setObjectName("ILampDialog")
        ILampDialog.resize(371, 203)
        ILampDialog.setMinimumSize(QtCore.QSize(371, 203))
        self.gridLayout_2 = QtWidgets.QGridLayout(ILampDialog)
        self.gridLayout_2.setObjectName("gridLayout_2")
        self.gridLayout = QtWidgets.QGridLayout()
        self.gridLayout.setObjectName("gridLayout")
        self.projectNameLabel = QtWidgets.QLabel(ILampDialog)
        self.projectNameLabel.setMinimumSize(QtCore.QSize(71, 21))
        self.projectNameLabel.setMaximumSize(QtCore.QSize(71, 21))
        self.projectNameLabel.setObjectName("projectNameLabel")
        self.gridLayout.addWidget(self.projectNameLabel, 0, 0, 1, 1)
        self.projectNameLineEdit = QtWidgets.QLineEdit(ILampDialog)
        self.projectNameLineEdit.setObjectName("projectNameLineEdit")
        self.gridLayout.addWidget(self.projectNameLineEdit, 0, 1, 1, 1)
        self.projectNameButton = QtWidgets.QPushButton(ILampDialog)
        self.projectNameButton.setMinimumSize(QtCore.QSize(31, 23))
        self.projectNameButton.setMaximumSize(QtCore.QSize(31, 23))
        self.projectNameButton.setObjectName("projectNameButton")
        self.gridLayout.addWidget(self.projectNameButton, 0, 2, 1, 1)
        self.inputFolderLabel = QtWidgets.QLabel(ILampDialog)
        self.inputFolderLabel.setMinimumSize(QtCore.QSize(71, 21))
        self.inputFolderLabel.setMaximumSize(QtCore.QSize(71, 21))
        self.inputFolderLabel.setObjectName("inputFolderLabel")
        self.gridLayout.addWidget(self.inputFolderLabel, 1, 0, 1, 1)
        self.inputFolderLineEdit = QtWidgets.QLineEdit(ILampDialog)
        self.inputFolderLineEdit.setObjectName("inputFolderLineEdit")
        self.gridLayout.addWidget(self.inputFolderLineEdit, 1, 1, 1, 1)
        self.inputFolderButton = QtWidgets.QPushButton(ILampDialog)
        self.inputFolderButton.setMinimumSize(QtCore.QSize(31, 23))
        self.inputFolderButton.setMaximumSize(QtCore.QSize(31, 23))
        self.inputFolderButton.setObjectName("inputFolderButton")
        self.gridLayout.addWidget(self.inputFolderButton, 1, 2, 1, 1)
        self.filepath2dLabel = QtWidgets.QLabel(ILampDialog)
        self.filepath2dLabel.setMinimumSize(QtCore.QSize(71, 21))
        self.filepath2dLabel.setMaximumSize(QtCore.QSize(71, 21))
        self.filepath2dLabel.setObjectName("filepath2dLabel")
        self.gridLayout.addWidget(self.filepath2dLabel, 2, 0, 1, 1)
        self.filepath2dLineEdit = QtWidgets.QLineEdit(ILampDialog)
        self.filepath2dLineEdit.setObjectName("filepath2dLineEdit")
        self.gridLayout.addWidget(self.filepath2dLineEdit, 2, 1, 1, 1)
        self.filepath2dButton = QtWidgets.QPushButton(ILampDialog)
        self.filepath2dButton.setMinimumSize(QtCore.QSize(31, 23))
        self.filepath2dButton.setMaximumSize(QtCore.QSize(31, 23))
        self.filepath2dButton.setObjectName("filepath2dButton")
        self.gridLayout.addWidget(self.filepath2dButton, 2, 2, 1, 1)
        self.filepathNdLabel = QtWidgets.QLabel(ILampDialog)
        self.filepathNdLabel.setMinimumSize(QtCore.QSize(71, 21))
        self.filepathNdLabel.setMaximumSize(QtCore.QSize(71, 21))
        self.filepathNdLabel.setObjectName("filepathNdLabel")
        self.gridLayout.addWidget(self.filepathNdLabel, 3, 0, 1, 1)
        self.filepathNdLineEdit = QtWidgets.QLineEdit(ILampDialog)
        self.filepathNdLineEdit.setObjectName("filepathNdLineEdit")
        self.gridLayout.addWidget(self.filepathNdLineEdit, 3, 1, 1, 1)
        self.filepathNdButton = QtWidgets.QPushButton(ILampDialog)
        self.filepathNdButton.setMinimumSize(QtCore.QSize(31, 23))
        self.filepathNdButton.setMaximumSize(QtCore.QSize(31, 23))
        self.filepathNdButton.setObjectName("filepathNdButton")
        self.gridLayout.addWidget(self.filepathNdButton, 3, 2, 1, 1)
        self.outputFolderLabel = QtWidgets.QLabel(ILampDialog)
        self.outputFolderLabel.setMinimumSize(QtCore.QSize(71, 21))
        self.outputFolderLabel.setMaximumSize(QtCore.QSize(71, 21))
        self.outputFolderLabel.setObjectName("outputFolderLabel")
        self.gridLayout.addWidget(self.outputFolderLabel, 4, 0, 1, 1)
        self.outputFolderLineEdit = QtWidgets.QLineEdit(ILampDialog)
        self.outputFolderLineEdit.setObjectName("outputFolderLineEdit")
        self.gridLayout.addWidget(self.outputFolderLineEdit, 4, 1, 1, 1)
        self.outputFolderButton = QtWidgets.QPushButton(ILampDialog)
        self.outputFolderButton.setMinimumSize(QtCore.QSize(31, 23))
        self.outputFolderButton.setMaximumSize(QtCore.QSize(31, 23))
        self.outputFolderButton.setObjectName("outputFolderButton")
        self.gridLayout.addWidget(self.outputFolderButton, 4, 2, 1, 1)
        self.gridLayout_2.addLayout(self.gridLayout, 0, 0, 1, 1)
        self.buttonBox = QtWidgets.QDialogButtonBox(ILampDialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtWidgets.QDialogButtonBox.Cancel|QtWidgets.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.gridLayout_2.addWidget(self.buttonBox, 1, 0, 1, 1)

        self.retranslateUi(ILampDialog)
        self.buttonBox.accepted.connect(ILampDialog.accept)
        self.buttonBox.rejected.connect(ILampDialog.reject)
        self.projectNameButton.clicked.connect(ILampDialog.onProjectNameClicked)
        self.inputFolderButton.clicked.connect(ILampDialog.onInputFolderClicked)
        self.outputFolderButton.clicked.connect(ILampDialog.onOutputFolderClicked)
        self.filepath2dButton.clicked.connect(ILampDialog.onFilePath2dClicked)
        self.filepathNdButton.clicked.connect(ILampDialog.onFilePathNdClicked)
        QtCore.QMetaObject.connectSlotsByName(ILampDialog)

    def retranslateUi(self, ILampDialog):
        _translate = QtCore.QCoreApplication.translate
        ILampDialog.setWindowTitle(_translate("ILampDialog", "ILamp Project Dialog"))
        self.projectNameLabel.setText(_translate("ILampDialog", "Project Name:"))
        self.projectNameButton.setText(_translate("ILampDialog", "..."))
        self.inputFolderLabel.setText(_translate("ILampDialog", "Input Folder:"))
        self.inputFolderButton.setText(_translate("ILampDialog", "..."))
        self.filepath2dLabel.setText(_translate("ILampDialog", "File Path 2d:"))
        self.filepath2dButton.setText(_translate("ILampDialog", "..."))
        self.filepathNdLabel.setText(_translate("ILampDialog", "File Path Nd:"))
        self.filepathNdButton.setText(_translate("ILampDialog", "..."))
        self.outputFolderLabel.setText(_translate("ILampDialog", "Ouput Folder:"))
        self.outputFolderButton.setText(_translate("ILampDialog", "..."))
