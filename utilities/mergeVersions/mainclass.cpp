/*
Merge Versions

Copyright (C) 2019 QLands Technology Consultants.
Author: Carlos Quiros (cquiros_at_qlands.com)

Merge Versions is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

Merge Versions is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with Merge Versions.  If not, see <http://www.gnu.org/licenses/lgpl-3.0.html>.
*/

#include "mainclass.h"
#include "mergecreate.h"
#include "compareinsert.h"

mainClass::mainClass(QObject *parent) : QObject(parent)
{
    returnCode = 0;
}

void mainClass::log(QString message)
{
    QString temp;
    temp = message + "\n";
    printf("%s",temp.toUtf8().data());
}

void mainClass::run()
{
    QDomDocument XMLResult;
    QDomElement XMLRoot;
    QDomElement eErrors;
    if (output_type != "h")
    {
        XMLResult = QDomDocument("XMLResult");
        XMLRoot = XMLResult.createElement("XMLResult");
        XMLResult.appendChild(XMLRoot);
        eErrors = XMLResult.createElement("errors");
        XMLRoot.appendChild(eErrors);
    }

    compareInsert insert;
    insert.setFiles(a_insertXML,b_insertXML,c_insertXML,d_insertSQL,output_type);
    returnCode = insert.compare();
    if (output_type != "h")
    {
        QList<TcompError> errorList = insert.getErrorList();
        for (int pos = 0; pos < errorList.count(); pos++)
        {
            QDomElement anError;
            anError = XMLResult.createElement("error");
            anError.setAttribute("table",errorList[pos].table);
            anError.setAttribute("field",errorList[pos].field);
            anError.setAttribute("code",errorList[pos].code);
            anError.setAttribute("from",errorList[pos].from);
            anError.setAttribute("to",errorList[pos].to);
            eErrors.appendChild(anError);
        }
    }
    if (returnCode == 0)
    {
        returnCode = insert.createCFile();
        if (returnCode == 0)
        {
            mergeCreate create;
            create.setFiles(a_createXML,b_createXML,c_createXML,d_createSQL,output_type);
            create.setInsertDiff(insert.getDiffs());
            returnCode = create.compare();
            if (output_type != "h")
            {
                QList<TcompError> errorList = create.getErrorList();
                for (int pos = 0; pos < errorList.count(); pos++)
                {
                    QDomElement anError;
                    anError = XMLResult.createElement("error");
                    anError.setAttribute("table",errorList[pos].table);
                    anError.setAttribute("field",errorList[pos].field);
                    anError.setAttribute("code",errorList[pos].code);
                    anError.setAttribute("from",errorList[pos].from);
                    anError.setAttribute("to",errorList[pos].to);
                    eErrors.appendChild(anError);
                }
            }
            if (returnCode == 0)
            {
                QStringList insertsUsed = create.getInsertTablesUsed();
                for (int pos = 0; pos < insertsUsed.count(); pos++)
                {
                    insert.setAsParsed(insertsUsed[pos]);
                }
            }
        }
    }
    if (returnCode == 0)
    {
        returnCode = insert.createDiffFile();
    }
    if (output_type != "h")
    {
        log(XMLResult.toString());
    }
    emit finished();
}
void mainClass::setParameters(QString createA, QString createB, QString insertA, QString insertB, QString createC, QString insertC, QString diffCreate, QString diffInsert, QString outputType)
{
    a_createXML = createA;
    b_createXML = createB;
    a_insertXML = insertA;
    b_insertXML = insertB;
    c_createXML = createC;
    c_insertXML = insertC;
    d_createSQL = diffCreate;
    d_insertSQL = diffInsert;
    output_type = outputType;
}
