const express = require("express"); // Initialize the express server instance.
const router = express.Router(); // Initialize the router

let deviceStatus = {};

const {
  showDatasets,
  showDatasetById,
  showDatasetsByFilter,
  createDataset,
  deleteDataset,
} = require("../controllers/mpp_controller.js"); // Imports the used controllers from the mpp_controllers.js file

router.get("/mppdata", showDatasets); // Get all datasets in the database and return them in JSON format.

router.get("/mppdata/:id", showDatasetById); // Show datasets by id, if id is null, show all datasets.

router.get("/mppdata/filter/:filter", showDatasetsByFilter); // Show datasets by filter, if filter is empty, show all datasets.

router.post("/mppdata", createDataset); // Create a new dataset

router.delete("/mppdata/:id", deleteDataset); // Delete a dataset by id and return the deleted dataset if successful.

module.exports = (io) => {
  router.post("/device/settings", (req, res) => {
    const data = req.body;
    io.emit("settings", data);
    res.sendStatus(200);
  });

  router.get("/device/status", (req, res) => {
    res.send(deviceStatus);
  });

  io.on("connection", (socket) => {
    console.log(`(Device): ${socket.id} connected`);

    socket.on("disconnect", () => {
      console.log(`(Device): ${socket.id} disconnected`);
      deviceStatus = {};
    });

    socket.on("status", (data) => {
      deviceStatus = data;
    });
  });

  return router;
};
