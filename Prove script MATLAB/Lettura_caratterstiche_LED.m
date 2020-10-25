NET.addAssembly('microsoft.office.interop.excel');
app = Microsoft.Office.Interop.Excel.ApplicationClass;
%book =  app.Workbooks.Open('H:\Documents\MATLAB\weight.xls');
book =  app.Workbooks.Open('D:\Università - magistrale\Progetto SMC-ACE\Prove script MATLAB\led01.csv');
sheet = Microsoft.Office.Interop.Excel.Worksheet(book.Worksheets.Item(1)); 
range = sheet.UsedRange;
arr = range.Value;

data = cell(arr,'ConvertTypes',{'all'});

cellfun(@disp,data(:,1))

Close(book)
Quit(app)