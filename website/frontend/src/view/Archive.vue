<template>
  <div class="box box-style">
    <h1 class="title">ARCHIV</h1>
    <DataList></DataList>
  </div>
</template>

<script>
import DataList from "../components/DataList.vue";

export default {
  name: "Archive",
  components: {
    DataList,
  },
};
</script>

<style>
.box-style {
  width: 800px;
}
</style>
