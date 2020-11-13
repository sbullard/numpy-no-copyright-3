"""Tests for :mod:`numpy.core.fromnumeric`."""

import numpy as np

A = np.array(True, ndmin=2, dtype=bool)
B = np.array(1.0, ndmin=2, dtype=np.float32)
A.setflags(write=False)
B.setflags(write=False)

a = np.bool_(True)
b = np.float32(1.0)
c = 1.0

np.take(a, 0)
np.take(b, 0)
np.take(c, 0)
np.take(A, 0)
np.take(B, 0)
np.take(A, [0])
np.take(B, [0])

np.reshape(a, 1)
np.reshape(b, 1)
np.reshape(c, 1)
np.reshape(A, 1)
np.reshape(B, 1)

np.choose(a, [True, True])
np.choose(A, [1.0, 1.0])

np.repeat(a, 1)
np.repeat(b, 1)
np.repeat(c, 1)
np.repeat(A, 1)
np.repeat(B, 1)

np.swapaxes(A, 0, 0)
np.swapaxes(B, 0, 0)

np.transpose(a)
np.transpose(b)
np.transpose(c)
np.transpose(A)
np.transpose(B)

np.partition(a, 0, axis=None)
np.partition(b, 0, axis=None)
np.partition(c, 0, axis=None)
np.partition(A, 0)
np.partition(B, 0)

np.argpartition(a, 0)
np.argpartition(b, 0)
np.argpartition(c, 0)
np.argpartition(A, 0)
np.argpartition(B, 0)

np.sort(A, 0)
np.sort(B, 0)

np.argsort(A, 0)
np.argsort(B, 0)

np.argmax(A)
np.argmax(B)
np.argmax(A, axis=0)
np.argmax(B, axis=0)

np.argmin(A)
np.argmin(B)
np.argmin(A, axis=0)
np.argmin(B, axis=0)

np.searchsorted(A[0], 0)
np.searchsorted(B[0], 0)
np.searchsorted(A[0], [0])
np.searchsorted(B[0], [0])

np.resize(a, (5, 5))
np.resize(b, (5, 5))
np.resize(c, (5, 5))
np.resize(A, (5, 5))
np.resize(B, (5, 5))

np.squeeze(a)
np.squeeze(b)
np.squeeze(c)
np.squeeze(A)
np.squeeze(B)

np.diagonal(A)
np.diagonal(B)

np.trace(A)
np.trace(B)

np.ravel(a)
np.ravel(b)
np.ravel(c)
np.ravel(A)
np.ravel(B)

np.nonzero(A)
np.nonzero(B)

np.shape(a)
np.shape(b)
np.shape(c)
np.shape(A)
np.shape(B)

np.compress([True], a)
np.compress([True], b)
np.compress([True], c)
np.compress([True], A)
np.compress([True], B)

np.clip(a, 0, 1.0)
np.clip(b, -1, 1)
np.clip(a, 0, None)
np.clip(b, None, 1)
np.clip(c, 0, 1)
np.clip(A, 0, 1)
np.clip(B, 0, 1)
np.clip(B, [0, 1], [1, 2])

np.sum(a)
np.sum(b)
np.sum(c)
np.sum(A)
np.sum(B)
np.sum(A, axis=0)
np.sum(B, axis=0)

np.all(a)
np.all(b)
np.all(c)
np.all(A)
np.all(B)
np.all(A, axis=0)
np.all(B, axis=0)
np.all(A, keepdims=True)
np.all(B, keepdims=True)

np.any(a)
np.any(b)
np.any(c)
np.any(A)
np.any(B)
np.any(A, axis=0)
np.any(B, axis=0)
np.any(A, keepdims=True)
np.any(B, keepdims=True)

np.cumsum(a)
np.cumsum(b)
np.cumsum(c)
np.cumsum(A)
np.cumsum(B)

np.ptp(b)
np.ptp(c)
np.ptp(B)
np.ptp(B, axis=0)
np.ptp(B, keepdims=True)

np.amax(a)
np.amax(b)
np.amax(c)
np.amax(A)
np.amax(B)
np.amax(A, axis=0)
np.amax(B, axis=0)
np.amax(A, keepdims=True)
np.amax(B, keepdims=True)

np.amin(a)
np.amin(b)
np.amin(c)
np.amin(A)
np.amin(B)
np.amin(A, axis=0)
np.amin(B, axis=0)
np.amin(A, keepdims=True)
np.amin(B, keepdims=True)
