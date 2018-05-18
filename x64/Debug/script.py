import sys

file = open('newts.csv', 'r')
output = open('output.txt', 'w')

line = file.readline()
s_line = line.split('\n')
labels = s_line[0].split(',')
columns = []
solutions = []
new = False

def percentage(solution, column):
	counter = 0
	for ind in solution:
		if ind in column:
			counter += 1
	return float(counter)/float(len(solution))

def normalize(vector, flag = False):
	for index, element in enumerate(vector):
		vector[index] = vector[index].replace('{', '')
		vector[index] = vector[index].replace('}', '')
		vector[index] = vector[index].split(';')
	if flag:
		for v in vector:
			print len(v)

def analyze(columns, solutions):
	normalize(columns, False)
	normalize(solutions, True)
	for column in columns:
		for solution in solutions:
			sys.stdout.write(str(percentage(solution, column)) + ',')
		sys.stdout.write('\n')

def find_index(labels, name):
	for index, label in enumerate(labels):
		if(label == name):
			return index
	return -1

index_reached = find_index(labels, 'reachedIDs')
index_columns = find_index(labels, 'columnIDs')
print 'SolutionIDs ... Index: ' + str(index_columns) + ' _ ' + labels[index_columns]
index_solution = find_index(labels, 'solutionIDs')
print 'SolutionIDs ... Index: ' + str(index_solution) + ' _ ' + labels[index_solution]

index_solution = index_columns

#solutions = ['0;1;6', '0;1;2']
#columns = ['0;1;2;3;4;5']
#analyze(columns, solutions)


for line in file:
	s_line = line.split('\n')
	s_line = s_line[0].split(',')
	if s_line[0] == '1':
		analyze(columns, solutions)
		#normalize(columns)
		#for index in range(0, len(columns)):
		#	columns[index] = sorted(columns[index])

		#for index, column in enumerate(columns):
		#	temp = open('temp' + str(index) + '.txt', 'w')
		#	temp.write(str(column) + '\nSize: ' + str(len(column)) + '\n')

		a = input('Enter')
		columns = []
		solutions = []
	else:
		columns.append(s_line[index_columns])
		solutions.append(s_line[index_solution])
