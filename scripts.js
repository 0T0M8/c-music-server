<script>
  function uploadFile() {
  const input = document.getElementById('fileInput');
  if (input.files.length === 0) {
    alert('Please select a file to upload.');
    return;
  }

  const file = input.files[0];
  const formData = new FormData();
  formData.append("file", file);

  fetch("/upload", {
    method: "POST",
    body: formData
  })
  .then(response => {
    if (response.ok) {
      alert("Upload successful!");
      // Optionally update playlist here
    } else {
      alert("Upload failed.");
    }
  })
  .catch(error => {
    alert("Error: " + error);
  });
}
</script>
