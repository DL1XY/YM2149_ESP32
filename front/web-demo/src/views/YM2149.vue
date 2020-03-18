<template>
  <v-container>
 </v-container>
</template>

<script>
export default {
  data() {
    return { red: 160, green: 160, blue: 160 };
  },
  methods: {
    YM2149_setChannelFreqFineHandler: function() {
      this.$ajax
        .post("/api/v1/light/brightness", {
          red: this.red,
          green: this.green,
          blue: this.blue
        })
        .then(data => {
          console.log(data);
        })
        .catch(error => {
          console.log(error);
        });
    }
  }
};
</script>