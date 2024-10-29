CREATE TABLE `devices` (
	`id` integer PRIMARY KEY AUTOINCREMENT NOT NULL,
	`sn` text NOT NULL,
	`latest_test` integer
);
--> statement-breakpoint
CREATE UNIQUE INDEX `devices_sn_unique` ON `devices` (`sn`);--> statement-breakpoint
CREATE TABLE `tests` (
	`id` integer PRIMARY KEY AUTOINCREMENT NOT NULL,
	`timestamp` integer NOT NULL,
	`sn_id` integer NOT NULL,
	`result` integer NOT NULL,
	FOREIGN KEY (`sn_id`) REFERENCES `devices`(`id`) ON UPDATE no action ON DELETE no action
);--> statement-breakpoint
CREATE TRIGGER update_latest_test_device
AFTER INSERT ON `tests`
FOR EACH ROW
BEGIN
  UPDATE `devices`
  SET `latest_test` = NEW.`id`
  WHERE `id` = NEW.`sn_id`;
END;
