# coding: utf-8
import xlrd
import sys
import json

result = {}
book = xlrd.open_workbook(sys.argv[1])
for sheet in book.sheets():
	result[sheet.name] = []
	for rownum in range(sheet.nrows):
		result[sheet.name].append(sheet.row_values(rownum))

print json.dumps(result)
