import json
import os

from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets import QApplication, QWidget, QInputDialog, QLineEdit, QFileDialog
from ui_ilampdialog import Ui_ILampDialog

class JsonSerializable(object):
    def toJson(self):
        return json.dumps(self.__dict__, indent=4)
    def __repr__(self):
        return self.toJson()


class Project(JsonSerializable):
    def __init__(self):
        self.ProjectName = ""
        self.FileName2d = "PyFilename.2d"
        self.FileNameNd = "PyFilename.nd"
        self.OutputFolder = "C:/tmp/"
        self.InputFiles = ["pymod1.ply", "pymod2.ply", "pymod3.ply"]
        self.NumNeighbours = 4
        self.KdTreeCount = 4
        self.KnnSearchChecks = 128




class ILampDialog(QtWidgets.QDialog, Ui_ILampDialog):
    def __init__(self):
        super(ILampDialog, self).__init__()

        # Set up the user interface from Designer.
        self.setupUi(self)
        self.project = Project()


    def onProjectNameClicked(self):
        fileName, _ = QFileDialog.getSaveFileName(self, "Save Project Name", "ProjectName.ilp", "ILamp Project Files (*.ilp)")
        if fileName:
            self.project.ProjectName = fileName
            self.projectNameLineEdit.setText(fileName)

            base, ext = os.path.splitext(fileName)

            self.project.FileName2d = base + ".2d"
            self.project.FileNameNd = base + ".nd"
            self.filepath2dLineEdit.setText(self.project.FileName2d)
            self.filepathNdLineEdit.setText(self.project.FileNameNd)

            self.project.OutputFolder = os.path.dirname(fileName)
            self.outputFolderLineEdit.setText(self.project.OutputFolder)


    def onInputFilesClicked(self):
        options = QtWidgets.QFileDialog.Options()
        files, _ = QtWidgets.QFileDialog.getOpenFileNames(self, "QFileDialog.getOpenFileNames()", "",
                                                          "PLY Files (*.ply);;Obj Files (*.obj);;All Files (*)",
                                                          options=options)
        if files:
            self.project.InputFiles = files
            self.inputFilesTextEdit.clear()
            for f in files:
                self.inputFilesTextEdit.append(f)


    def onFilePath2dClicked(self):
        fileName, _ = QFileDialog.getSaveFileName(self, "QFileDialog.getSaveFileName()", "", "2d Project Files (*.2d)")
        if fileName:
            self.project.FileName2d = fileName
            self.filepath2dLineEdit.setText(fileName)


    def onFilePathNdClicked(self):
        fileName, _ = QFileDialog.getSaveFileName(self, "QFileDialog.getSaveFileName()", "", "Nd Project Files (*.nd)")
        if fileName:
            self.project.FileNameNd = fileName
            self.filepathNdLineEdit.setText(fileName)


    def onOutputFolderClicked(self):
        options = QtWidgets.QFileDialog.Options()
        options |= QFileDialog.ShowDirsOnly | QFileDialog.DontResolveSymlinks
        folder = QtWidgets.QFileDialog.getExistingDirectory(self, "Select Output Folder", "/home", options=options)
        if folder:
            self.project.OutputFolder = folder
            self.outputFolderLineEdit.setText(folder)


    def accept(self):
        super(ILampDialog, self).accept()

        self.project.NumNeighbours = self.numNeighboursSpinBox.value()
        self.project.KdTreeCount = self.kdTreeCountSpinBox.value()
        self.project.KnnSearchChecks = self.knnSearchChecksSpinBox.value()

        if self.project.ProjectName:
            with open(self.project.ProjectName, 'w') as outfile:
                outfile.write("%s" % self.project.toJson())


    def reject(self):
        super(ILampDialog, self).reject()


if __name__ == "__main__":
    import sys

    app = QtWidgets.QApplication(sys.argv)
    img_dlg = ILampDialog()
    img_dlg.show()
    sys.exit(app.exec_())
