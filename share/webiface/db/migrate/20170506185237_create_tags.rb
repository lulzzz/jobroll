class CreateTags < ActiveRecord::Migration[5.0]
  def change
    create_table :tags do |t|
      t.integer :uid
      t.integer :size
      t.binary  :dump

      t.timestamps
    end
    add_index :tags, :uid, unique: true
  end
end
