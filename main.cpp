#include <chrono>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include <expresscpp/expresscpp.hpp>

#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/std.h>

#include <httplib.h>

#include <nlohmann/json.hpp>

using json = nlohmann::json;
static constexpr auto port = 3000U;
//
struct city
{
  std::string name;
  std::pair<double, double> coords;
};

// Mapping of country -> (city -> (latitude, longitude))
static const std::map<std::string, std::array<city, 3>> locations = {
  { "Poland",
    { city { "Warsaw", { 52.2297, 21.0122 } },
      city { "Krakow", { 50.0647, 19.9450 } },
      city { "Wroclaw", { 51.1079, 17.0385 } } } },
  { "USA",
    { city { "New York", { 40.7128, -74.0060 } },
      city { "Los Angeles", { 34.0522, -118.2437 } },
      city { "Chicago", { 41.8781, -87.6298 } } } },
  { "UK",
    { city { "London", { 51.5074, -0.1278 } },
      city { "Manchester", { 53.4808, -2.2426 } },
      city { "Edinburgh", { 55.9533, -3.1883 } } } },
  { "Japan",
    { city { "Tokyo", { 35.6895, 139.6917 } },
      city { "Osaka", { 34.6937, 135.5023 } },
      city { "Kyoto", { 35.0116, 135.7681 } } } }
};

auto
fetch_weather(double latitude, double longitude) -> json
{
  std::string url = fmt::format(
    "/v1/forecast?latitude={}&longitude={}&current_weather=true", latitude,
    longitude);

  httplib::Client cli("api.open-meteo.com");
  auto res = cli.Get(url);
  if (!res || res->status != 200) { fmt::println("Failed to fetch weather"); }
  return json::parse(res->body);
}

auto
main() -> std::int32_t
{
  const auto time = std::chrono::system_clock::now();
  fmt::println("{}", time);
  fmt::println("Konrad Nowak");
  fmt::println("port: {}", port);

  auto app = std::make_shared<expresscpp::ExpressCpp>();

  // Single endpoint: form + result
  app->Get(
    "/",
    [](auto req, auto res)
    {
      auto params = req->GetQueryParams();
      std::string html = "<h1>Select Location</h1>"
                         "<form method=\"get\" action=\"/\">"
                         "<label>Country: <select name=\"country\" "
                         "onchange=\"this.form.submit()\">";
      html += "<option value=\"\">-- Select Country --</option>";
      for (const auto& [ country, cities ] : locations)
      {
        html += fmt::format(
          "<option value=\"{}\"{}>{}</option>", country,
          (params[ "country" ] == country ? " selected" : ""), country);
      }
      html += "</select></label><br>";

      html += "<label>City: <select name=\"city\">";
      html += "<option value=\"\">-- Select City --</option>";
      if (!params[ "country" ].empty())
      {
        auto country_it = locations.find(params[ "country" ]);
        if (country_it != locations.end())
        {
          for (const auto& [ city, coords ] : country_it->second)
          {
            html += fmt::format(
              "<option value=\"{}\"{}>{}</option>", city,
              (params[ "city" ] == city ? " selected" : ""), city);
          }
        }
      }
      html += "</select></label><br>";

      html += "<button type=\"submit\">Get Weather</button>";
      html += "</form>";

      if (!params[ "country" ].empty() && !params[ "city" ].empty())
      {
        auto country_it = locations.find(params[ "country" ]);
        if (country_it != locations.end())
        {
          auto city_it = std::find_if(
            country_it->second.begin(), country_it->second.end(),
            [ &params ](const city& city)
            { return city.name == params[ "city" ]; });
          if (city_it != country_it->second.end())
          {
            try
            {
              const auto& selected_country = params[ "country" ];
              const auto& selected_city = params[ "city" ];
              const auto& [ lat, lon ] = city_it->coords;
              auto weather = fetch_weather(lat, lon);
              double temperature =
                weather[ "current_weather" ][ "temperature" ];
              double wind_speed = weather[ "current_weather" ][ "windspeed" ];
              html += fmt::format(
                "<h2>Weather for {}, {}</h2>"
                "<p>Temperature: {}</p>"
                "<p>Wind Speed: {}</p>",
                selected_city, selected_country, temperature, wind_speed);
            }
            catch (const std::exception& ex)
            {
              html += fmt::format("<p>Error: {}</p>", ex.what());
            }
          }
        }
      }

      res->Send(html);
    });

  app
    ->Listen(
      port,
      [](auto ec)
      {
        if (ec) { fmt::println("Error starting server: {}", ec.message()); }
      })
    .Run();
}
