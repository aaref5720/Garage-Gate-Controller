package com.example.automotivedashboard

import android.os.Bundle
import android.widget.Button
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import okhttp3.*
import okhttp3.MediaType.Companion.toMediaType
import okhttp3.RequestBody.Companion.toRequestBody
import org.json.JSONObject
import java.io.IOException
import java.util.concurrent.TimeUnit

/**
 * MainActivity for Automotive Dashboard app.
 * Handles garage commands using HTTP.
 *
 * @author Abdelrahman Mohamed
 */
class MainActivity : AppCompatActivity() {

    private lateinit var connectionStatusTextView: TextView
    private lateinit var httpClient: OkHttpClient
    
    // CC3200 HTTP server configuration
    private val cc3200BaseUrl = "http://192.168.0.105"  // CC3200 IP (port 80 is default)
    private val pcServerUrl = "http://192.168.0.103:8080"  // Your PC server IP and port

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        connectionStatusTextView = findViewById(R.id.connectionStatusTextView)

        val openBtn = findViewById<Button>(R.id.openGarageButton)
        val closeBtn = findViewById<Button>(R.id.closeGarageButton)

        openBtn.setOnClickListener { sendGarageCommand("open") }
        closeBtn.setOnClickListener { sendGarageCommand("close") }

        // Initialize HTTP client
        httpClient = OkHttpClient.Builder()
            .connectTimeout(10, TimeUnit.SECONDS)
            .readTimeout(10, TimeUnit.SECONDS)
            .writeTimeout(10, TimeUnit.SECONDS)
            .build()

        // Test connection to CC3200
        testConnection()
    }

    /**
     * Tests connection to the CC3200 HTTP server.
     */
    private fun testConnection() {
        connectionStatusTextView.text = "HTTP Status: Testing connection..."
        
        val request = Request.Builder()
            .url("$cc3200BaseUrl/")
            .get()
            .build()

        httpClient.newCall(request).enqueue(object : Callback {
            override fun onFailure(call: Call, e: IOException) {
                runOnUiThread {
                    connectionStatusTextView.text = "HTTP Status: Connection Failed"
                    Toast.makeText(this@MainActivity, "Cannot connect to CC3200: ${e.message}", Toast.LENGTH_LONG).show()
                }
            }

            override fun onResponse(call: Call, response: Response) {
                runOnUiThread {
                    if (response.isSuccessful) {
                        connectionStatusTextView.text = "HTTP Status: Connected to CC3200"
                        Toast.makeText(this@MainActivity, "Connected to CC3200", Toast.LENGTH_SHORT).show()
                    } else {
                        connectionStatusTextView.text = "HTTP Status: Connection Error"
                        Toast.makeText(this@MainActivity, "CC3200 returned error: ${response.code}", Toast.LENGTH_LONG).show()
                    }
                }
            }
        })
    }

    /**
     * Sends a garage command via HTTP to the CC3200.
     * @param action The action to perform ("open" or "close").
     */
    private fun sendGarageCommand(action: String) {
        val json = JSONObject().apply {
            put("command", action)
            put("timestamp", System.currentTimeMillis())
        }
        
        val requestBody = json.toString().toRequestBody("application/json".toMediaType())
        
        val request = Request.Builder()
            .url("$cc3200BaseUrl/$action")
            .post(requestBody)
            .build()

        println("Sending $action command to: $cc3200BaseUrl/$action")
        println("Request body: ${json.toString()}")

        httpClient.newCall(request).enqueue(object : Callback {
            override fun onFailure(call: Call, e: IOException) {
                println("HTTP request failed: ${e.message}")
                e.printStackTrace()
                runOnUiThread {
                    Toast.makeText(this@MainActivity, "Error sending $action command: ${e.message}", Toast.LENGTH_LONG).show()
                }
            }

            override fun onResponse(call: Call, response: Response) {
                val responseBody = response.body?.string() ?: ""
                println("HTTP response: ${response.code} - $responseBody")
                
                runOnUiThread {
                    if (response.isSuccessful) {
                        Toast.makeText(this@MainActivity, "Garage $action command sent successfully!", Toast.LENGTH_SHORT).show()
                        
                        // Also send to PC server for logging
                        sendToPcServer(action)
                    } else {
                        Toast.makeText(this@MainActivity, "CC3200 returned error: ${response.code} - $responseBody", Toast.LENGTH_LONG).show()
                    }
                }
            }
        })
    }

    /**
     * Sends command to PC server for logging and monitoring.
     * @param action The action performed ("open" or "close").
     */
    private fun sendToPcServer(action: String) {
        val json = JSONObject().apply {
            put("command", action)
            put("source", "android_app")
            put("timestamp", System.currentTimeMillis())
        }
        
        val requestBody = json.toString().toRequestBody("application/json".toMediaType())
        
        val request = Request.Builder()
            .url("$pcServerUrl/$action")
            .post(requestBody)
            .build()

        httpClient.newCall(request).enqueue(object : Callback {
            override fun onFailure(call: Call, e: IOException) {
                // PC server is optional, don't show error to user
                println("PC server not reachable: ${e.message}")
            }

            override fun onResponse(call: Call, response: Response) {
                println("Command logged to PC server: $action")
            }
        })
    }

    override fun onDestroy() {
        super.onDestroy()
        httpClient.dispatcher.executorService.shutdown()
        httpClient.connectionPool.evictAll()
    }
}
