CREATE TRIGGER update_latest_test_device
AFTER INSERT ON `tests`
FOR EACH ROW
BEGIN
  UPDATE `devices`
  SET `latest_test` = NEW.`id`
  WHERE `id` = NEW.`sn_id`;
END;--> statement-breakpoint

CREATE TRIGGER latest_test_device_constraint_update
AFTER UPDATE ON `tests`
FOR EACH ROW
BEGIN
  UPDATE `devices`
  SET `latest_test` = (
    SELECT `id`
    FROM `tests`
    ORDER BY `tests`.`timestamp`
    LIMIT 1
  )
  WHERE `id` = NEW.`sn_id`;
END;--> statement-breakpoint

CREATE TRIGGER latest_test_device_constraint_delete
AFTER DELETE ON `tests`
FOR EACH ROW
BEGIN
  UPDATE `devices`
  SET `latest_test` = (
    SELECT `id`
    FROM `tests`
    ORDER BY `tests`.`timestamp`
    LIMIT 1
  )
  WHERE `id` = NEW.`sn_id`;
END;
