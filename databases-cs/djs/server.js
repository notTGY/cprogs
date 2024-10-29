import {
  lastTests,
  lastTestsByDevice,
  faultyDevices,
} from './db.js'


console.log(
  await lastTestsByDevice('S1919012464')
)

console.log(
  await lastTests()
)

console.log(
  await faultyDevices()
)
