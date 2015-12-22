# coding: utf-8
import xlrd
import sys

book = xlrd.open_workbook(sys.argv[1])
for sheet in book.sheets():
	print sheet.name
	for rownum in range(sheet.nrows):
		print "\t".join(sheet.row_values(rownum))

