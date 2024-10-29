import { text, integer, sqliteTable, primaryKey } from "drizzle-orm/sqlite-core"

export const devices = sqliteTable('devices', {
  id: integer('id').primaryKey({ autoIncrement: true }),
  sn: text('sn').unique().notNull(),
  latest_test: integer('latest_test'),
})
export const tests = sqliteTable('tests', {
  id: integer('id').primaryKey({ autoIncrement: true }),
  timestamp: integer('timestamp').notNull(),
  sn_id: integer('sn_id')
    .notNull().references(() => devices.id),
  result: integer('result').notNull(),
})

