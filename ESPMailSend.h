#include <ESP_Mail_Client.h>
#define SMTP_HOST "smtp.mail.yahoo.com"
#define SMTP_PORT esp_mail_smtp_port_587
/* The log in credentials */
#define AUTHOR_EMAIL "mochammad.effendi@yahoo.com"
#define AUTHOR_PASSWORD "pqbyhuxthwoooetw"

/* The SMTP Session object used for Email sending */
SMTPSession smtp;

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);

const char rootCACert[] PROGMEM = "-----BEGIN CERTIFICATE-----\n"
                                  "-----END CERTIFICATE-----\n";

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status)
{
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success())
  {
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++)
    {
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients.c_str());
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject.c_str());
    }
    Serial.println("----------------\n");

    //You need to clear sending result as the memory usage will grow up.
    smtp.sendingResult.clear();
  }
}

void smtpsend(String _msgtxt, String _infosubject){
  smtp.debug(1);
    /* Set the callback function to get the sending results */
    smtp.callback(smtpCallback);
    /* Declare the session config data */
    ESP_Mail_Session session;
    /* Set the session config */
    session.server.host_name = SMTP_HOST;
    session.server.port = SMTP_PORT;
    session.login.email = AUTHOR_EMAIL;
    session.login.password = AUTHOR_PASSWORD;
    session.login.user_domain = F("yahoo.com");
    /* Set the NTP config time */
    session.time.ntp_server = F("pool.ntp.org,time.nist.gov");
    session.time.gmt_offset = 7;
    session.time.day_light_offset = 0;
    /* Declare the message class */
    SMTP_Message message;
    /* Set the message headers */
    message.sender.name = F("Deauther Fluxion");
    message.sender.email = AUTHOR_EMAIL;
    message.subject = _infosubject;
    message.addRecipient(F("Mochammad Effendi"), F("mochammad.effendi@yahoo.com"));
    String textMsg = _msgtxt;
    message.text.content = textMsg;
    message.text.charSet = F("us-ascii");
    message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
    message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
    /* Set the custom message header */
    message.addHeader(F("Message-ID: <abcde.fghij@gmail.com>"));
    /* Connect to server with the session config */
    if (!smtp.connect(&session))
      return;
    /* Start sending Email and close the session */
    if (!MailClient.sendMail(&smtp, &message))
      Serial.println("Error sending Email, " + smtp.errorReason());
    //to clear sending result log
    //smtp.sendingResult.clear();
    ESP_MAIL_PRINTF("Free Heap: %d\n", MailClient.getFreeHeap());
}
