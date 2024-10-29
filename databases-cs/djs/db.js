import {
  or,
  eq,
  notInArray,
  inArray,
  desc,
} from 'drizzle-orm'
import { drizzle } from 'drizzle-orm/better-sqlite3'
import Database from 'better-sqlite3'

import { devices, tests } from './schema.js'
import { resolve } from 'node:path'

const dbPath = resolve('./db/sqlite.db')
const sqlite = new Database(dbPath)
const logger = false
const db = drizzle(sqlite, { logger })

export const faultyDevices = async () => {
  return await db.select()
    .from(devices)
    .leftJoin(tests, eq(devices.latest_test, tests.id))
    .where(eq(tests.result, 0))
}

export const lastTestsByDevice = async (sn, N=10) => {
  return await db.transaction(async tx => {
    const deviceVector = await tx.select()
      .from(devices)
      .where(eq(devices.sn, sn))
      .limit(1)

    if (deviceVector.length === 0) {
      tx.rollback()
      return []
    }
    const sn_id = deviceVector[0].id

    return tx.select()
      .from(tests)
      .where(eq(tests.sn_id, sn_id))
      .orderBy(desc(tests.timestamp))
      .limit(N)
  })
}

export const lastTests = async (N=10) => {
  return await db.transaction(async tx => {
    return tx.select()
      .from(tests)
      .orderBy(desc(tests.timestamp))
      .limit(N)
  })
}

export const insert = async (obj) => {
  const { timestamp, sn, result } = obj

  let device = await db
    .select()
    .from(devices)
    .where(eq(devices.sn, sn))
    .limit(1)

  if (device.length === 0) {
    device = await db.insert(devices).values({
      sn,
    }).returning()
  }

  const sn_id = device[0].id

  await db.insert(tests).values({
    timestamp,
    sn_id,
    result,
  })
}

export const insertMany = async (objs) => {
  await db.transaction(async tx => {
    const sns = objs.map(o => o.sn)
    const devicesIn = await tx.select()
      .from(devices)
      .where(inArray(devices.sn, sns))
    
    const devicesToInsert = 
      sns.filter(s => !devicesIn.find(d => d.sn === s))
    const uniqueDevicesToInsert = [...new Set(devicesToInsert)]

    //console.log('inserting %d devices', devicesToInsert.length)

    let deviceInserted = []
    if (uniqueDevicesToInsert.length > 0) {
      deviceInserted = await tx.insert(devices)
        .values(uniqueDevicesToInsert.map(sn => ({sn})))
        .returning()
    }

    const testsToInsert = await Promise.all(
      objs.map(async o => {
        const { timestamp, sn, result } = o

        let sn_id = deviceInserted.find(d => d.sn === sn)

        if (sn_id == null) {
          /*
          const sns = await tx.select()
            .from(devices)
            .where(eq(devices.sn, sn))
            .limit(1)
          sn_id = sns[0].id
          */
          sn_id = devicesIn.find(d => d.sn === sn)
        }

        
        return {
          timestamp,
          sn_id: sn_id.id,
          result
        }
      })
    )

    //console.log('inserting %d devices', testsToInsert.length)
    await tx.insert(tests).values(testsToInsert)
  })
}
