<template>
  <div class="box box-style">
    <h1 class="title">MESSUNG</h1>
    <apexchart
      type="area"
      height="350"
      :options="chartOptions"
      :series="series"
    ></apexchart>
  </div>
  <div class="box box-style">
    <h1 class="title">EINSTELLUNG DER MESSUNG</h1>
    <table class="table is-striped is-bordered is-narrow is-hoverable is-fullwidth">
      <thead>
        <tr>
          <th>Stage</th>
          <th>Auswahl der Lichschranke</th>
          <th>Verzögerungszeit [us]</th>
        </tr>
      </thead>
      <tbody>
        <tr v-for="index in this.msData.n_stage" :key="index">
          <td>{{ index }}</td>
          <td v-if="this.msData.msdata[index - 1].lsSelect">Vorherige</td>
          <td v-else>Eigene</td>
          <td>{{ this.msData.msdata[index - 1].dtSelect }}</td>
        </tr>
      </tbody>
    </table>
  </div>
</template>

<script>
import axios from "axios";
import { series, chartOptions } from "../assets/chart.js";
import { stageLength, firstLength } from "../assets/const.js";
import { useCookies } from "vue3-cookies";

export default {
  name: "ViewData",
  setup() {
    const { cookies } = useCookies();
    return { cookies };
  },
  data() {
    return {
      msData: {},
      series,
      chartOptions,
    };
  },
  created: function () {
    if (this.$route.params.id) {
      this.cookies.set("id", this.$route.params.id);
    }
    this.getDataById();
  },
  methods: {
    async getDataById() {
      try {
        /**
         * Make a request to the given URL and return the response.
         * @param {string} url - the URL to make the request to
         * @returns The response from the request.
         */
        const response = await axios.get(
          `https://pmpp.info/mppdata/${this.cookies.get("id")}`
        );
        this.msData = response.data;
        let vel = [];
        let acc = [];
        /**
         * Calculates the velocity of the stage for each element in the response.
         * @param {object} response - the response from the API
         * @param {number} index - the index of the stage in the response
         * @returns None
         */
        let index = 0;
        for (let element of response.data.msdata) {
          if (index === 0) {
            vel[index] = firstLength / (element.msData / 1000000);
          } else {
            vel[index] = stageLength / (element.msData / 1000000);
          }
          vel[index] = vel[index].toFixed(2);
          index++;
        }
        /**
         * Calculates the acceleration of the given data.
         * @param {number[]} vel - The velocity of the data.
         * @param {number} index - The index of the current data.
         * @returns The acceleration of the data.
         */
        index = 0;
        for (let element of response.data.msdata) {
          if (index === 0) {
            acc[index] = vel[index] / (element.msData / 1000000);
          } else {
            acc[index] = (vel[index] - vel[index - 1]) / (element.msData / 1000000);
          }
          acc[index] = acc[index].toFixed(2);
          index++;
        }
        /**
         * Creates a series of data points for the velocity and acceleration graphs.
         * @param {number[]} vel - The velocity data points.
         * @param {number[]} acc - The acceleration data points.
         * @returns None
         */
        this.series = [
          {
            data: vel,
          },
          {
            data: acc,
          },
        ];
        let timestamp = new Date(response.data.timestamp);
        response.data.timestamp = {};
        response.data.timestamp.date = timestamp.toLocaleDateString();
        response.data.timestamp.time = timestamp.toLocaleTimeString();
        let titleText =
          "Messung vom " +
          response.data.timestamp.date +
          " um " +
          response.data.timestamp.time;
        /**
         * Sets the title of the chart.
         * @param {string} titleText - the text to set the title to.
         * @returns None
         */
        this.chartOptions = {
          title: {
            text: titleText,
          },
        };
      } catch (err) {
        console.log(err);
      }
    },
  },
};
</script>

<style>
.box-style {
  width: 800px;
}
</style>
