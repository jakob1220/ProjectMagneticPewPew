<template>
  <table class="table is-bordered is-striped is-narrow is-hoverable is-fullwidth">
    <thead>
      <tr>
        <th>Datum</th>
        <th>Uhrzeit</th>
        <th>Anzahl der Stages</th>
        <th>Endgeschwindigkeit</th>
        <th class="has-text-centered">Aktionen</th>
      </tr>
    </thead>
    <tbody>
      <tr v-for="item in items" :key="item.id">
        <td style="width: 20%">{{ item.timestamp.date }}</td>
        <td style="width: 20%">{{ item.timestamp.time }}</td>
        <td style="width: 20%">{{ item.n_stage }}</td>
        <td style="width: 20%">{{ item.max_velocity + " m/s" }}</td>
        <td style="width: 20%">
          <div class="is-flex is-justify-content-space-evenly">
            <router-link
              :to="{ name: 'View', params: { id: item.id } }"
              class="button is-small is-info is-rounded button-style"
            >
              <span class="icon">
                <i class="fa fa-search" />
              </span>
              <span> Anzeigen </span>
            </router-link>
            <a class="button is-danger is-small is-rounded" @click="deleteData(item.id)">
              <span class="icon">
                <i class="fa fa-trash-alt" />
              </span>
              <span> Löschen </span>
            </a>
          </div>
        </td>
      </tr>
    </tbody>
  </table>
</template>

<script>
// import axios
import axios from "axios";
import { stageLength, firstLength } from "../assets/const.js";

export default {
  name: "DataList",
  components: {},
  data() {
    return {
      items: [],
    };
  },
  created() {
    this.getData();
  },
  mounted() {
    this.showData();
  },
  methods: {
    /**
     * A function that calls the getData function every 200 milliseconds.
     * @returns None
     */
    showData() {
      setInterval(this.getData, 200);
    },
    /**
     * Gets the data from the server and stores it in the items array.
     * @returns None
     */
    async getData() {
      try {
        /**
         * Make a request to the given URL.
         * @param {string} url - the URL to make the request to
         * @returns {Promise<AxiosResponse>} - the response from the request
         */
        const response = await axios.get("https://pmpp.info/mppdata");
        this.items = response.data;
        /**
         * Takes in an array of objects, converts the timestamp object to a readable format
         * and calculates velocity and acceleration from measurement data.
         * @param {Array<Object>} items - the array of objects to add a timestamp to.
         * @returns None
         */
        for (let element of this.items) {
          let timestamp = new Date(element.timestamp);
          element.timestamp = {};
          element.timestamp.date = timestamp.toLocaleDateString("uk-Uk");
          element.timestamp.time = timestamp.toLocaleTimeString();
          if (element.n_stage === 1) {
            element.max_velocity = firstLength / (element.max_velocity / 1000000);
          } else {
            element.max_velocity = stageLength / (element.max_velocity / 1000000);
          }
          element.max_velocity = element.max_velocity.toFixed(2);
        }
      } catch (err) {
        console.log(err);
      }
    },
    /**
     * Deletes the data for the given id.
     * @param {string} id - the id of the data to delete.
     * @returns None
     */
    async deleteData(id) {
      try {
        await axios.delete(`https://pmpp.info/mppdata/${id}`);
        this.getData();
      } catch (err) {
        console.log(err);
      }
    },
  },
};
</script>

<style>
.button-style {
  background-color: hsl(207, 89%, 68%) !important;
}
</style>
