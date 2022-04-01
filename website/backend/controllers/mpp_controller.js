// Import function from db_querys
const {
  getDatasets,
  getDatasetById,
  getDatasetsByFilter,
  insertDataset,
  deleteDatasetById,
} = require("../models/mpp_model.js");

/**
 * Returns a list of all datasets.
 * @returns A list of all datasets
 */
exports.showDatasets = (req, res) => {
  getDatasets((err, results) => {
    if (err) {
      res.send(err);
    } else {
      res.json(results);
    }
  });
};

/**
 * Returns the dataset with the given id.
 * @param req.params.id - The id of the returned dataset.
 * @returns The dataset with matching id.
 */
exports.showDatasetById = (req, res) => {
  getDatasetById(req.params.id, (err, results) => {
    if (err) {
      res.send(err);
    } else {
      res.json(results[0]);
    }
  });
};

/**
 * Returns a list of datasets that match the given filter.
 * @param filter - The filter to use.
 * @returns A list of datasets that match the given filter.
 */
exports.showDatasetsByFilter = (req, res) => {
  getDatasetsByFilter(req.params.filter, (err, results) => {
    if (err) {
      res.send(err);
    } else {
      res.json(results);
    }
  });
};

/**
 * Takes in a dataset and inserts it into the database.
 * @param req.body - The dataset to insert.
 * @returns An error or success message.
 */
exports.createDataset = (req, res) => {
  insertDataset(req.body, (err, results) => {
    if (err) {
      res.send(err);
    } else {
      res.json(results);
    }
  });
};

/**
 * Deletes a dataset by id.
 * @param req.params.id - The id of the deleted dataset.
 * @returns An error or success message.
 */
exports.deleteDataset = (req, res) => { 
  deleteDatasetById(req.params.id, (err, results) => {
    if (err) {
      res.send(err);
    } else {
      res.json(results);
    }
  });
};
