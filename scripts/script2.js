function createTypingEffect(elementId, words, typingSpeed = 80, pauseTime = 500) {
  const element = document.getElementById(elementId);

  let wordIndex = 0;
  let charIndex = 0;
  let typing = true;

  function typeEffect() {
    const currentWord = words[wordIndex];

    if (typing) {
      if (charIndex < currentWord.length) {
        element.textContent += currentWord.charAt(charIndex);
        charIndex++;
      } else {
        setTimeout(() => typing = false, pauseTime);
      }
    } else {
      if (charIndex > 0) {
        element.textContent = currentWord.substring(0, charIndex - 1);
        charIndex--;
      } else {
        typing = true;
        wordIndex = (wordIndex + 1) % words.length;
      }
    }
  }

  setInterval(typeEffect, typingSpeed);
}

createTypingEffect("word1", ["Autopsy.", "HEX Editor.", "FTK Imager.", "Wireshark.", "Burp Suite.", "Armitage.", "Metasploit."]);

createTypingEffect("word2", ["Secure Web Technologies.", "Internet of Things.", "Digital Incident Scene Investigation and Analysis.", "Computer Networks.", "Project Management and Professional Practice.", "Programming for Data Communication.", "Information in Organizations."]);
