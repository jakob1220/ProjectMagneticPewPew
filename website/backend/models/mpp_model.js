const db = require("../config/database.js");

/**
 * Gets all of the datasets from the database.
 * @param result - The callback function to call with the results.
 * @returns A list of all datasets
 */
exports.getDatasets = (result) => {
  db.query("SELECT * FROM pmpp", (err, results) => {
    if (err) {
      console.log(err);
      result(err, null);
    } else {
      result(null, results);
    }
  });
};

/**
 * Get the dataset with the given id.
 * @param id - The id of the dataset to get.
 * @param result - The callback function to call with the results.
 * @returns The dataset with matching id.
 */
exports.getDatasetById = (id, result) => {
  db.query("SELECT * FROM pmpp WHERE id = ?", [id], (err, results) => {
    if (err) {
      console.log(err);
      result(err, null);
    } else {
      result(null, results);
    }
  });
};

/**
 * Get the datasets that match the given filter.
 * @param filter - The filter to apply.
 * @param result - The callback function to call with the results.
 * @returns A list of datasets that match the given filter.
 */
exports.getDatasetsByFilter = (filter, result) => {
  filter = JSON.parse(filter);

  /**
   * Checks if the max-velocity filter is valid.
   * @param filter - The filter object.
   * @param filter.max - maximum value of max_velocity to filter for.
   * @param filter.min - minimum value of max_velocity to filter for.
   * @returns None if valid. Throws error when invalid.
   */
  if(Object.keys(filter).length === 0) {
    filter = {"n_stage": null, "max_velocity": {"min": null, "max": null}}
  }
  else if (
    filter.n_stage < 0 ||
    (filter.max_velocity.max < filter.max_velocity.min &&
      filter.max_velocity.min !== null &&
      filter.max_velocity.max !== null)
  )
    throw "ERROR with filter: file: mpp_model.js ~ line 54 ~ filter";

  db.query(
    "SELECT * FROM pmpp WHERE n_stage = IFNULL(?, n_stage) AND max_velocity >= IFNULL(?, max_velocity) AND max_velocity <= IFNULL(?, max_velocity)",
    [filter.n_stage, filter.max_velocity.min, filter.max_velocity.max],
    (err, results) => {
      if (err) {
        console.log(err);
        result(err, null);
      } else {
        result(null, results);
      }
    }
  );
};

/**
 * Inserts the given data into the database.
 * @param data - The data to insert.
 * @param result - The callback function to call when the query is complete.
 * @returns An error or success message.
 */
exports.insertDataset = (data, result) => {
  data.msdata = JSON.stringify(data.msdata);

  if (data.n_stage === 0) throw "ERROR with data-insert: n_stage cannot be 0";

  db.query("INSERT INTO pmpp SET ?", [data], (err, results) => {
    if (err) {
      console.log(err);
      result(err, null);
    } else {
      result(null, results);
    }
  });
};

/**
 * Deletes a dataset from the database.
 * @param id - The id of the dataset to delete.
 * @param result - The callback function to call when the query is complete.
 * @returns An error or success message.
 */
exports.deleteDatasetById = (id, result) => {
  db.query("DELETE FROM pmpp WHERE id = ?", [id], (err, results) => {
    if (err) {
      console.log(err);
      result(err, null);
    } else {
      result(null, results);
    }
  });
};
