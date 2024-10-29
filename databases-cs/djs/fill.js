import { insert, insertMany } from './db.js'
import { readFileSync } from 'node:fs'

/**
 * Returns chunks of size n.
 * @param {Array<any>} array any array
 * @param {number} n size of chunk 
 */
function* chunks(array, n){
  for(let i = 0; i < array.length; i += n) yield array.slice(i, i + n);
}


const fill = async () => {
  const file = readFileSync(
    '../dev-tests-large.csv',
    'utf8',
  )
  const lines = file.split('\n')
  const objects = lines.map(str => {
    const fields = str.split(',')
    const obj = {
      timestamp: Number(fields[0]),
      sn: fields[1], 
      result: Number(fields[2]),
    }
    return obj
  }).filter(o => {
    return !(o.timestamp == null || o.sn == null || o.result == null)
  })

  const N = 10000
  for (const chunk of chunks(objects, N)) {
    await insertMany(chunk)
    console.log(chunk.length)
  }
  //await insertMany(objects)
  
  console.log('Inserted %d tests', objects.length)
}

fill()
