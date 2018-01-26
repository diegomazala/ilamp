from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets import QApplication, QWidget, QInputDialog, QLineEdit, QFileDialog
from ui_ilampdialog import Ui_ILampDialog


class ILampDialog(QtWidgets.QDialog, Ui_ILampDialog):
    def __init__(self):
        super(ILampDialog, self).__init__()

        # Set up the user interface from Designer.
        self.setupUi(self)


        self.inputFiles = []
        self.projectFileName = "Project.ilp"
        self.filepath2d = "Filename.2d"
        self.filepathNd = "Filename.nd"
        self.outputFolder = "/home"


    def onProjectNameClicked(self):
        fileName, _ = QFileDialog.getSaveFileName(self, "QFileDialog.getSaveFileName()", "", "ILamp Project Files (*.ilp)")
        if fileName:
            self.projectFileName = fileName
            self.projectNameLineEdit.setText(fileName)


    def onInputFolderClicked(self):
        options = QtWidgets.QFileDialog.Options()
        files, _ = QtWidgets.QFileDialog.getOpenFileNames(self, "QFileDialog.getOpenFileNames()", "",
                                                          "PLY Files (*.ply);;Obj Files (*.obj);;All Files (*)",
                                                          options=options)
        if files:
            self.inputFiles = files


    def onFilePath2dClicked(self):
        fileName, _ = QFileDialog.getSaveFileName(self, "QFileDialog.getSaveFileName()", "", "2d Project Files (*.2d)")
        if fileName:
            self.filepath2d = fileName
            self.filepath2dLineEdit.setText(fileName)


    def onFilePathNdClicked(self):
        fileName, _ = QFileDialog.getSaveFileName(self, "QFileDialog.getSaveFileName()", "", "Nd Project Files (*.nd)")
        if fileName:
            self.filepathNd = fileName
            self.filepathNdLineEdit.setText(fileName)


    def onOutputFolderClicked(self):
        options = QtWidgets.QFileDialog.Options()
        options |= QFileDialog.ShowDirsOnly | QFileDialog.DontResolveSymlinks
        folder = QtWidgets.QFileDialog.getExistingDirectory(self, "Select Output Folder", "/home", options=options)
        if folder:
            self.outputFolder = folder
            self.outputFolderLineEdit.setText(folder)


    def accept(self):
        super(ILampDialog, self).accept()
        print(self.projectFileName)
        print(self.outputFolder)


    def reject(self):
        super(ILampDialog, self).reject()
        print(self.projectFileName)
        print(self.outputFolder)


if __name__ == "__main__":
    import sys

    app = QtWidgets.QApplication(sys.argv)
    img_dlg = ILampDialog()
    img_dlg.show()
    sys.exit(app.exec_())
