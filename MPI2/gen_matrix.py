import numpy as np
import sys

N = int(sys.argv[1])
print(N)

A = np.random.rand(N, N) * 10.0
A = np.dot(A, A.T)

with open('A{}x{}.txt'.format(N, N), 'w') as f:
    f.write('{} {}\n'.format(N, N))
    for j in range(N):
        for i in range(N):
            if i == j:
                A[i, j] += 1
            f.write('{} '.format(A[i, j]))
        f.write('\n')

b = np.dot(A, np.ones(N))

with open('b{}x{}.txt'.format(1, N), 'w') as f:
    f.write('{} {}\n'.format(1, N))
    for i in range(N):
        f.write('{} '.format(b[i]))
    f.write('\n')
