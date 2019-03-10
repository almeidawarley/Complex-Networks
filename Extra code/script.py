file = open('criterio5.txt', 'r')
counter = 0
for line in file:
	words = line.split()
	if (len(words) > 1):
		counter += 1
		if counter != int(words[0]):
			print(words[0])
			a = input('enter')