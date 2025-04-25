-- 1) Функция остаётся без изменений
CREATE OR REPLACE FUNCTION notify_change_with_id() RETURNS trigger AS $$
DECLARE
  rec_id text;
BEGIN
  rec_id := COALESCE(NEW.id::text, OLD.id::text);
  PERFORM pg_notify(
    'table_changes',
    json_build_object(
      'table', TG_TABLE_NAME,
      'action', TG_OP,
      'id',     rec_id
    )::text
  );
  IF (TG_OP = 'DELETE') THEN
    RETURN OLD;
  ELSE
    RETURN NEW;
  END IF;
END;
$$ LANGUAGE plpgsql;

-- 2) DO‑блок, который берёт только BASE TABLE с колонкой id
DO $$
DECLARE
  tbl RECORD;
BEGIN
  FOR tbl IN
    SELECT c.table_name
    FROM information_schema.columns   AS c
    JOIN information_schema.tables    AS t
      ON c.table_schema = t.table_schema
     AND c.table_name   = t.table_name
    WHERE c.table_schema = 'public'
      AND c.column_name  = 'id'
      AND t.table_type   = 'BASE TABLE'
  LOOP
    EXECUTE format(
      'DROP TRIGGER IF EXISTS trg_change_with_id ON %I;',
      tbl.table_name
    );
    EXECUTE format($fmt$
      CREATE TRIGGER trg_change_with_id
        AFTER INSERT OR UPDATE OR DELETE
        ON %I
        FOR EACH ROW
        EXECUTE FUNCTION notify_change_with_id();
    $fmt$, tbl.table_name);
  END LOOP;
END;
$$;
