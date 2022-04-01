<template>
  <nav class="navbar nav-style p-2" role="navigation" aria-label="main navigation">
    <div class="navbar-brand">
      <a class="navbar-item" href="https://github.com/jakob1220/ProjectMagneticPewPew">
        <img src="../assets/kek.png" width="" height="" />
      </a>
    </div>

    <div class="navbar-menu">
      <div class="navbar-end">
        <router-link :to="{ name: 'Dashboard' }" class="navbar-item">
          Home
        </router-link>

        <router-link :to="{ name: 'Dashboard' }" class="navbar-item">
          Documentation
        </router-link>
      </div>
    </div>
  </nav>
</template>

<script>
export default {
  name: "NavBar",
};
</script>

<style>
.nav-style {
  width: 100%;
  height: 90px;
  width: 100%;
}

</style>
