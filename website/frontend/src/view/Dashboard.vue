<template>
  <div class="box box-style">
    <h1 class="title">STATUS</h1>
    <div v-if="this.renderDashboardItems">
      <div>
        <p class="subtitle">Anzahl der Stages: {{ status.n_stage }}</p>
        <br />
        <table class="table is-bordered is-striped is-narrow is-hoverable is-fullwidth">
          <thead>
            <tr>
              <th style="width: 10%">Stage</th>
              <th style="width: 90%">Ladestatus</th>
            </tr>
          </thead>
          <tbody>
            <tr v-for="index in this.settings.n_stage" :key="index">
              <td>{{ index }}</td>
              <td v-if="status.stage[index - 1].onLoad">geladen</td>
              <td v-else>nicht geladen</td>
            </tr>
          </tbody>
        </table>
      </div>
    </div>
    <div v-else>
      <h2 class="subtitle">Gerät offline</h2>
    </div>
  </div>
  <div class="box box-style">
    <h1 class="title">EINSTELLUNGEN</h1>
    <div v-if="this.renderDashboardItems">
      <table class="table is-bordered is-striped is-narrow is-hoverable is-fullwidth">
        <thead>
          <tr>
            <th style="width: 10%">Stage</th>
            <th style="width: 45%">Lichtschranke</th>
            <th style="width: 45%">Verzögerungszeit [us]</th>
          </tr>
        </thead>
        <tbody>
          <tr v-for="index in this.settings.n_stage" :key="index">
            <td>{{ index }}</td>
            <td>
              <div class="select is-rounded is-small is-fullwidth">
                <select
                  v-model="this.settings.stage[index - 1].lsSelect"
                  class="is-fullwidth"
                >
                  <option v-bind:value="false">Eigene</option>
                  <option v-bind:value="true">Vorherige</option>
                </select>
              </div>
            </td>
            <td>
              <input
                class="input is-rounded is-small"
                type="number"
                min="0"
                max="4294967296"
                v-model="this.settings.stage[index - 1].dtSelect"
              />
            </td>
          </tr>
        </tbody>
      </table>
      <a class="button is-small is-info is-rounded button-style" @click="sendSettings()">
        <span class="icon">
          <i class="fa fa-arrow-alt-circle-right" />
        </span>
        <span>Senden</span>
      </a>
    </div>
    <div v-else>
      <h2 class="subtitle">Gerät offline</h2>
    </div>
  </div>
</template>

<script>
import axios from "axios";

export default {
  name: "Settings",
  data() {
    return {
      status: {},
      settings: {
        n_stage: Number,
        stage: [],
      },
      renderDashboardItems: false,
    };
  },
  created() {
    this.getStatus();
  },
  mounted() {
    this.showStatus();
  },
  methods: {
    /**
     * Sends the settings to the server.
     * @returns None
     */
    async sendSettings() {
      try {
        /**
         * Send a POST request to the server with the given settings.
         * @param {object} body - The settings to send to the server.
         * @returns The response from the server.
         */
        const response = await axios.post("https://pmpp.info/device/settings/", {
          body: this.settings,
        });
        console.log(response);
        /**
         * Initializes the stage settings array.
         * @returns None
         */
        for (let index = 0; index < this.settings.n_stage; index++) {
          this.settings.stage[index] = { lsSelect: 0, dtSelect: 0 };
        }
      } catch (err) {
        console.log(err);
      }
    },
    async getStatus() {
      try {
        /**
         * Make a request to the given URL and return the response.
         * @param {string} url - the URL to make the request to.
         * @returns {Promise<AxiosResponse>} - the response from the request.
         */
        const response = await axios.get(`https://pmpp.info/device/status`);
        /**
         * Checks if the number of stages has changed and updates the stage settings accordingly.
         * @param {number} n_stage - the number of stages in the current experiment.
         * @returns None
         */
        if (this.status.n_stage !== response.data.n_stage) {
          this.settings.n_stage = response.data.n_stage;
          this.settings.stage = new Array(response.data.n_stage);
          /**
           * Initializes the stage settings array.
           * @returns None
           */
          for (let index = 0; index < response.data.n_stage; index++) {
            this.settings.stage[index] = { lsSelect: 0, dtSelect: 0 };
          }
        }
        this.status = response.data;
        /**
         * Checks if the response from the API has a n_stage property.
         * @param {object} response - the response from the API.
         * @returns None
         */
        if (response.data.n_stage) {
          this.renderDashboardItems = true;
        } else {
          this.renderDashboardItems = false;
        }
      } catch (err) {
        console.log(err);
      }
    },
    /**
     * Gets the status of the extension and updates the status bar.
     * @returns None
     */
    showStatus() {
      setInterval(this.getStatus, 200);
    },
  },
};
</script>

<style>
.button-style {
  background-color: hsl(207, 89%, 68%) !important;
}
.box-style {
  width: 800px;
}
</style>
